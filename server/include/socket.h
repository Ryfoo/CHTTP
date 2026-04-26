#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "http.h"

#define QUEUE_LIMIT 1024

void          address_init(struct sockaddr_in* addr, const char* port);
void          address_init_ip(struct sockaddr_in* addr,
                              const char* ip, const char* port);
socket_fd_t   listening_starter(struct sockaddr_in* addr);

/*
   Accept loop: blocks accepting connections and dispatches each to a
   detached worker thread that handles the keep-alive lifecycle.
*/
void          monitor(socket_fd_t host_fd, volatile int* running);


/* Client side — kept for the existing client driver. */
socket_fd_t   connection_starter(const struct sockaddr_in* addr);
n_bytes_t     exchange(socket_fd_t fd,
                       const struct sockaddr_in* addr,
                       const char* method,
                       const char* uri,
                       header_t headers[HEADERS_LEN],
                       size_t headers_count,
                       const char* body,
                       http_response_t* res);

#endif
