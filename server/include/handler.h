#pragma once

#ifndef HANDLER_H
#define HANDLER_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include "common.h"

// user-defined constants

n_bytes_t handle_http_request(char* buffer);
n_bytes_t handle_http_response();

#endif