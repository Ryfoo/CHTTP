
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <string.h>
#include "common.h"
#include <stdio.h>


success_flag_t serialize(http_request_t req, char* buffer, size_t buffer_size);

#endif