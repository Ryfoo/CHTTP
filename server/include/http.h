
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"


#ifndef HTTP_H
#define HTTP_H




success_flag_t request_init(http_request_t *req, char* method, char* uri, header_t headers[HEADERS_LEN], size_t headers_count, char* body);
void request_add_header(http_request_t *req, const char *name, const char *value);
void request_set_body(http_request_t *req, const char *data, size_t len);



#endif