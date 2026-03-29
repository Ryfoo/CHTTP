
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "parser.h"
#include "serializer.h"
#include "router.h"

#ifndef HTTP_H
#define HTTP_H




void request_add_header(http_request_t *req, const char *name, const char *value);
void request_set_body(http_request_t *req, const char *data, size_t len);



success_flag_t request_init(
                            http_request_t *req, 
                            const char* method, 
                            const char* uri, 
                            header_t headers[HEADERS_LEN], 
                            const size_t headers_count,
                            const char* body
                        );
success_flag_t handle_http_request( 
                                    const char* recv_buffer,
                                    char* send_buffer,
                                    http_request_t* req,
                                    http_response_t* res
                                );




#endif