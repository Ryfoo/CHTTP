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



success_flag_t index_handler(http_request_t req, http_response_t* res);

#endif