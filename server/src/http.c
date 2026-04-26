#include "../include/http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>


void http_request_free(http_request_t* req)
{
    if (!req) return;
    free(req->req_line);
    free(req->head);
    free(req->body);
    req->req_line = NULL;
    req->head = NULL;
    req->body = NULL;
    req->body_size = 0;
}

void http_response_free(http_response_t* res)
{
    if (!res) return;
    free(res->res_line);
    free(res->head);
    free(res->body);
    res->res_line = NULL;
    res->head = NULL;
    res->body = NULL;
    res->body_size = 0;
}


success_flag_t http_build_response(http_request_t* req,
                                   http_response_t* res,
                                   int keep_alive)
{
    if (!req || !res) return FAILURE;

    /* Dispatch. The handler allocates res->res_line / res->head / res->body. */
    if (router_dispatch(req, res) != SUCCESS) {
        /* Handler bailed without producing anything → synthesise a 500.
           If even that fails we have nothing to send. */
        http_response_free(res);
        if (make_500(res) != SUCCESS) return FAILURE;
    }

    /* Always advertise HTTP/1.1 — that's the contract of this server. */
    strcpy(res->res_line->http_version, "HTTP/1.1");

    /* Standard headers added on every response. */
    char date[64];
    http_date_now(date, sizeof(date));
    header_set(res->head, "Date", date);
    header_set(res->head, "Server", "scratchy/1.1");
    header_set(res->head, "Connection", keep_alive ? "keep-alive" : "close");

    char clbuf[32];
    snprintf(clbuf, sizeof(clbuf), "%zu", res->body_size);
    header_set(res->head, "Content-Length", clbuf);

    return SUCCESS;
}


/* Used by the client driver. */
success_flag_t request_init(http_request_t* req,
                            const char* method,
                            const char* uri,
                            header_t headers[HEADERS_LEN],
                            const size_t headers_count,
                            const char* body)
{
    if (!req || !method || !uri) return FAILURE;
    if (headers_count > HEADERS_LEN) return FAILURE;

    req->req_line = calloc(1, sizeof(request_line_t));
    req->head     = calloc(1, sizeof(headers_list_t));
    req->body     = NULL;
    req->body_size = 0;
    if (!req->req_line || !req->head) goto fail;

    strncpy(req->req_line->method, method, SHORT_LENGTH - 1);
    strncpy(req->req_line->uri,    uri,    URI_LENGTH   - 1);
    strcpy (req->req_line->http_version, "HTTP/1.1");

    if (headers && headers_count) {
        memcpy(req->head->headers, headers, headers_count * sizeof(header_t));
        req->head->headers_counter = headers_count;
    }

    if (body && *body) {
        size_t n = strlen(body);
        req->body = malloc(n + 1);
        if (!req->body) goto fail;
        memcpy(req->body, body, n + 1);
        req->body_size = n;
    }
    return SUCCESS;

fail:
    http_request_free(req);
    return FAILURE;
}
