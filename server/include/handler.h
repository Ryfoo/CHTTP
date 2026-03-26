#pragma once

#ifndef HANDLER_H
#define HANDLER_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include "../include/common.h"

// user-defined constants

n_bytes_t handle_client(socket_fd_t fd);

typedef int socket_fd_t;
typedef struct http_request http_request_t;
typedef struct http_response http_response_t;
typedef int n_bytes_t;

#endif