#ifndef HTTP_H
#define HTTP_H

#include "common.h"
#include "parser.h"
#include "serializer.h"
#include "router.h"
#include "handler.h"

/*
   Builds a server-side response to `req`. Adds Date / Server /
   Content-Length / Connection headers based on `keep_alive`.

   On return, `res` is fully owned by the caller and must be freed with
   http_response_free().
*/
success_flag_t http_build_response(http_request_t* req,
                                   http_response_t* res,
                                   int keep_alive);

void http_request_free (http_request_t*  req);
void http_response_free(http_response_t* res);


/* Kept for the existing client driver. */
success_flag_t request_init(http_request_t* req,
                            const char* method,
                            const char* uri,
                            header_t headers[HEADERS_LEN],
                            const size_t headers_count,
                            const char* body);

#endif
