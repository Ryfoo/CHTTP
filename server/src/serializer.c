#include <stdio.h>
#include <string.h>
#include "../include/serializer.h"

success_flag_t serialize(http_request_t req, char* buffer, size_t buffer_size) {
    size_t offset = 0;
    int written;

    if (!buffer || buffer_size == 0)
        return FAILURE;

    buffer[0] = '\0';

    // Request line: METHOD URI VERSION\r\n
    written = snprintf(buffer + offset, buffer_size - offset,
                       "%s %s %s\r\n",
                       req.req_line->method, req.req_line->uri, req.req_line);

    if (written < 0 || (size_t)written >= buffer_size - offset)
        return FAILURE;

    offset += written;

    // Headers
    for (size_t i = 0; i < HEADERS_LEN; i++) {
        written = snprintf(buffer + offset, buffer_size - offset,
                           "%s: %s\r\n",
                           req.headers[i].name,
                           req.headers[i].value
                        );

        if (written < 0 || (size_t)written >= buffer_size - offset)
            return FAILURE;

        offset += written;
    }

    // Empty line between headers and body
    written = snprintf(buffer + offset, buffer_size - offset, "\r\n");

    if (written < 0 || (size_t)written >= buffer_size - offset)
        return FAILURE;

    offset += written;

    // Body (optional)
    if (req.body) {
        written = snprintf(buffer + offset, buffer_size - offset,
                           "%s", req.body);

        if (written < 0 || (size_t)written >= buffer_size - offset)
            return FAILURE;

        offset += written;
    }

    return SUCCESS;
}



