#include "../include/socket.h"
#include "../include/http.h"
#include "../include/parser.h"
#include "../include/serializer.h"
#include "../include/utils.h"

#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <strings.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* Idle keep-alive timeout per connection. */
#define KEEPALIVE_TIMEOUT_S   15
#define KEEPALIVE_MAX_REQS    100


/* ──────────────────────────────────────────────────────────────────────── */
/*  Address & listener                                                      */
/* ──────────────────────────────────────────────────────────────────────── */

void address_init(struct sockaddr_in* addr, const char* port)
{
    address_init_ip(addr, "0.0.0.0", port);
}

void address_init_ip(struct sockaddr_in* addr, const char* ip, const char* port)
{
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port   = htons((uint16_t)atoi(port));

    /* Empty / "*" / "0.0.0.0" all mean "every interface". */
    if (!ip || !*ip || strcmp(ip, "*") == 0 || strcmp(ip, "0.0.0.0") == 0) {
        addr->sin_addr.s_addr = INADDR_ANY;
        return;
    }
    if (inet_pton(AF_INET, ip, &addr->sin_addr) != 1) {
        fprintf(stderr,
                "warning: '%s' is not a valid IPv4 — binding 0.0.0.0\n", ip);
        addr->sin_addr.s_addr = INADDR_ANY;
    }
}

socket_fd_t listening_starter(struct sockaddr_in* addr)
{
    socket_fd_t fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return FAILURE; }

    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    if (bind(fd, (struct sockaddr*)addr, sizeof(*addr)) < 0) {
        perror("bind"); close(fd); return FAILURE;
    }
    if (listen(fd, QUEUE_LIMIT) < 0) {
        perror("listen"); close(fd); return FAILURE;
    }
    return fd;
}


/* ──────────────────────────────────────────────────────────────────────── */
/*  IO helpers                                                              */
/* ──────────────────────────────────────────────────────────────────────── */

static n_bytes_t send_all(socket_fd_t fd, const void* data, size_t n)
{
    const char* p = data;
    size_t left = n;
    while (left > 0) {
        ssize_t w = send(fd, p, left, MSG_NOSIGNAL);
        if (w < 0) {
            if (errno == EINTR) continue;
            return FAILURE;
        }
        if (w == 0) return FAILURE;
        p    += w;
        left -= (size_t)w;
    }
    return (n_bytes_t)n;
}

/* Read exactly `n` bytes (used for the body once we know Content-Length). */
static n_bytes_t recv_exact(socket_fd_t fd, void* out, size_t n)
{
    char* p = out;
    size_t left = n;
    while (left > 0) {
        ssize_t r = recv(fd, p, left, 0);
        if (r < 0) {
            if (errno == EINTR) continue;
            return FAILURE;
        }
        if (r == 0) return FAILURE; /* peer closed */
        p    += r;
        left -= (size_t)r;
    }
    return (n_bytes_t)n;
}

/*
   Read until CRLFCRLF. On success:
     *head_out = malloc'd, NUL-terminated string up to and including CRLFCRLF
     *head_len = byte length (without the trailing NUL)
     *leftover_out / *leftover_len  = bytes already read past the CRLFCRLF
                                      (these belong to the body and must be
                                      consumed before further recv() calls)
   Returns FAILURE on EOF / timeout / oversize.
*/
static success_flag_t read_head(socket_fd_t fd,
                                char** head_out, size_t* head_len,
                                char** leftover_out, size_t* leftover_len)
{
    char* buf = malloc(REQ_HEAD_MAX);
    if (!buf) return FAILURE;

    size_t total = 0;
    char*  marker = NULL;

    while (total < REQ_HEAD_MAX - 1) {
        ssize_t r = recv(fd, buf + total, REQ_HEAD_MAX - 1 - total, 0);
        if (r < 0) {
            if (errno == EINTR) continue;
            free(buf); return FAILURE;
        }
        if (r == 0) { free(buf); return FAILURE; }
        total += (size_t)r;
        buf[total] = '\0';

        marker = strstr(buf, "\r\n\r\n");
        if (marker) break;
    }
    if (!marker) { free(buf); return FAILURE; }

    size_t head_bytes = (size_t)(marker - buf) + 4; /* include CRLFCRLF */
    char*  head = malloc(head_bytes + 1);
    if (!head) { free(buf); return FAILURE; }
    memcpy(head, buf, head_bytes);
    head[head_bytes] = '\0';

    size_t left = total - head_bytes;
    char*  spill = NULL;
    if (left > 0) {
        spill = malloc(left);
        if (!spill) { free(head); free(buf); return FAILURE; }
        memcpy(spill, buf + head_bytes, left);
    }

    free(buf);
    *head_out      = head;
    *head_len      = head_bytes;
    *leftover_out  = spill;
    *leftover_len  = left;
    return SUCCESS;
}


/* ──────────────────────────────────────────────────────────────────────── */
/*  Per-request work                                                        */
/* ──────────────────────────────────────────────────────────────────────── */

static int wants_close(const http_request_t* req)
{
    /* HTTP/1.1 default = keep-alive. HTTP/1.0 default = close. */
    int default_close =
        (strcasecmp(req->req_line->http_version, "HTTP/1.1") != 0);

    const char* conn = header_get(req->head, "Connection");
    if (!conn) return default_close;

    if (strcasecmp(conn, "close") == 0)      return 1;
    if (strcasecmp(conn, "keep-alive") == 0) return 0;
    return default_close;
}

/*
   Returns:
     1  -> served, keep the connection open
     0  -> served, close the connection
    -1  -> client closed / read failure / unrecoverable
*/
static int serve_one(socket_fd_t fd, int allow_keep_alive)
{
    char*  head_buf = NULL;
    size_t head_len = 0;
    char*  spill    = NULL;
    size_t spill_n  = 0;

    if (read_head(fd, &head_buf, &head_len, &spill, &spill_n) != SUCCESS) {
        return -1;
    }

    http_request_t  req = {0};
    http_response_t res = {0};

    if (parse_req_head(head_buf, &req) != SUCCESS) {
        free(head_buf); free(spill);
        /* 400 Bad Request, then close. */
        if (make_status_response(&res, 400, "Bad Request",
                                 "400 — Bad Request\n") == SUCCESS) {
            strcpy(res.res_line->http_version, "HTTP/1.1");
            char date[64]; http_date_now(date, sizeof(date));
            header_set(res.head, "Date", date);
            header_set(res.head, "Server", "scratchy/1.1");
            header_set(res.head, "Connection", "close");
            char clbuf[32]; snprintf(clbuf, sizeof(clbuf), "%zu", res.body_size);
            header_set(res.head, "Content-Length", clbuf);

            char head_out[SEND_HEAD_MAX];
            n_bytes_t hn = serialize_res_head(head_out, sizeof(head_out), &res);
            if (hn > 0) {
                send_all(fd, head_out, (size_t)hn);
                if (res.body && res.body_size) send_all(fd, res.body, res.body_size);
            }
            http_response_free(&res);
        }
        return -1;
    }
    free(head_buf);

    /* Read the body if the request advertises Content-Length. */
    size_t cl = 0;
    const char* clh = header_get(req.head, "Content-Length");
    if (clh) {
        long v = atol(clh);
        if (v < 0 || (size_t)v > BODY_MAX) {
            /* 413 and close. */
            free(spill);
            http_request_free(&req);
            if (make_status_response(&res, 413, "Payload Too Large",
                                     "413 — Payload Too Large\n") == SUCCESS) {
                strcpy(res.res_line->http_version, "HTTP/1.1");
                header_set(res.head, "Connection", "close");
                char clbuf[32]; snprintf(clbuf, sizeof(clbuf), "%zu", res.body_size);
                header_set(res.head, "Content-Length", clbuf);
                char head_out[SEND_HEAD_MAX];
                n_bytes_t hn = serialize_res_head(head_out, sizeof(head_out), &res);
                if (hn > 0) {
                    send_all(fd, head_out, (size_t)hn);
                    send_all(fd, res.body, res.body_size);
                }
                http_response_free(&res);
            }
            return -1;
        }
        cl = (size_t)v;
    }

    if (cl > 0) {
        char* body = malloc(cl + 1);
        if (!body) { free(spill); http_request_free(&req); return -1; }

        size_t copied = spill_n < cl ? spill_n : cl;
        if (copied) memcpy(body, spill, copied);

        if (copied < cl) {
            if (recv_exact(fd, body + copied, cl - copied) < 0) {
                free(body); free(spill);
                http_request_free(&req);
                return -1;
            }
        }
        body[cl] = '\0';
        req.body = body;
        req.body_size = cl;
    }
    free(spill);

    /* Decide keep-alive before building the response. */
    int keep = allow_keep_alive && !wants_close(&req);

    if (http_build_response(&req, &res, keep) != SUCCESS) {
        http_request_free(&req);
        http_response_free(&res);
        return -1;
    }

    /* Send head, then body. Body is sent as raw bytes (binary-safe). */
    char head_out[SEND_HEAD_MAX];
    n_bytes_t hn = serialize_res_head(head_out, sizeof(head_out), &res);
    if (hn < 0) {
        http_request_free(&req); http_response_free(&res);
        return -1;
    }

    int send_ok = (send_all(fd, head_out, (size_t)hn) >= 0);
    if (send_ok && res.body && res.body_size > 0) {
        send_ok = (send_all(fd, res.body, res.body_size) >= 0);
    }

    /* Brief access log: METHOD URI -> STATUS bytes */
    fprintf(stdout, "[%s] %s %s -> %d %zu\n",
            keep ? "k" : "c",
            req.req_line->method,
            req.req_line->uri,
            res.res_line->status,
            res.body_size);
    fflush(stdout);

    http_request_free(&req);
    http_response_free(&res);

    if (!send_ok) return -1;
    return keep ? 1 : 0;
}


/* ──────────────────────────────────────────────────────────────────────── */
/*  Worker thread                                                           */
/* ──────────────────────────────────────────────────────────────────────── */

static void* worker(void* arg)
{
    socket_fd_t fd = (socket_fd_t)(intptr_t)arg;

    /* Idle read timeout for keep-alive. */
    struct timeval tv = { .tv_sec = KEEPALIVE_TIMEOUT_S, .tv_usec = 0 };
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    int served = 0;
    while (served < KEEPALIVE_MAX_REQS) {
        int rc = serve_one(fd, /*allow_keep_alive=*/1);
        served++;
        if (rc <= 0) break;       /* close or error */
    }

    shutdown(fd, SHUT_WR);
    close(fd);
    return NULL;
}


/* ──────────────────────────────────────────────────────────────────────── */
/*  Accept loop                                                             */
/* ──────────────────────────────────────────────────────────────────────── */

void monitor(socket_fd_t host_fd, volatile int* running)
{
    /* Make sure dead peers don't kill us. */
    signal(SIGPIPE, SIG_IGN);

    while (running == NULL || *running) {
        struct sockaddr_in peer;
        socklen_t plen = sizeof(peer);
        socket_fd_t cfd = accept(host_fd, (struct sockaddr*)&peer, &plen);
        if (cfd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        pthread_t th;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        if (pthread_create(&th, &attr, worker,
                           (void*)(intptr_t)cfd) != 0)
        {
            perror("pthread_create");
            close(cfd);
        }
        pthread_attr_destroy(&attr);
    }
}


/* ──────────────────────────────────────────────────────────────────────── */
/*  Client side (kept for the client driver)                                */
/* ──────────────────────────────────────────────────────────────────────── */

socket_fd_t connection_starter(const struct sockaddr_in* addr)
{
    if (!addr) return FAILURE;
    socket_fd_t fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return FAILURE; }
    if (connect(fd, (const struct sockaddr*)addr, sizeof(*addr)) < 0) {
        perror("connect"); close(fd); return FAILURE;
    }
    return fd;
}

n_bytes_t exchange(socket_fd_t fd,
                   const struct sockaddr_in* addr,
                   const char* method,
                   const char* uri,
                   header_t headers[HEADERS_LEN],
                   size_t headers_count,
                   const char* body,
                   http_response_t* res)
{
    (void)addr;
    if (fd < 0 || !method || !uri || !res) return FAILURE;

    http_request_t req = {0};
    if (request_init(&req, method, uri, headers, headers_count, body) != SUCCESS)
        return FAILURE;

    /* Add Host + Content-Length so HTTP/1.1 servers respond. */
    if (!header_get(req.head, "Host")) header_set(req.head, "Host", "127.0.0.1");
    if (req.body_size) {
        char clbuf[32]; snprintf(clbuf, sizeof(clbuf), "%zu", req.body_size);
        header_set(req.head, "Content-Length", clbuf);
    }
    header_set(req.head, "Connection", "close");

    char head[SEND_HEAD_MAX];
    n_bytes_t hn = serialize_req_head(head, sizeof(head), &req);
    if (hn < 0) { http_request_free(&req); return FAILURE; }

    if (send_all(fd, head, (size_t)hn) < 0) { http_request_free(&req); return FAILURE; }
    if (req.body_size && send_all(fd, req.body, req.body_size) < 0) {
        http_request_free(&req); return FAILURE;
    }
    http_request_free(&req);

    /* Read the entire response into a growable buffer. */
    size_t cap = 8192, len = 0;
    char* buf = malloc(cap);
    if (!buf) return FAILURE;
    for (;;) {
        if (len + 4096 > cap) {
            cap *= 2;
            char* nb = realloc(buf, cap);
            if (!nb) { free(buf); return FAILURE; }
            buf = nb;
        }
        ssize_t r = recv(fd, buf + len, cap - len - 1, 0);
        if (r < 0) { if (errno == EINTR) continue; free(buf); return FAILURE; }
        if (r == 0) break;
        len += (size_t)r;
    }
    buf[len] = '\0';

    success_flag_t prc = parse_res(buf, res);
    free(buf);
    if (prc != SUCCESS) return FAILURE;
    return (n_bytes_t)len;
}
