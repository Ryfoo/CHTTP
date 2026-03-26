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

n_bytes_t http_handler(char* buffer);
n_bytes_t handel_http_response();

#endif