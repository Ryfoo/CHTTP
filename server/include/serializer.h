#include <stdio.h>
#include <string.h>
#include "common.h"

#ifndef SERIALIZER_H
#define SERIALIZER_H

/**
 * takes as arguments
 * @params http_request_t req, the type is defined at server/include/common.h.
 * @params char* buffer that will be used to save serialized data.
 * @params size_t buffer_size will be used as a memory size limit.
 * returns 0 on success, -1 on failure.
 */
success_flag_t serialize_req(http_request_t req, char* buffer, size_t buffer_size);

#endif