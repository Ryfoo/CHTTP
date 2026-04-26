#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>


#define FAILURE -1
#define SUCCESS  0

/*
   Buffer sizes.
   - REQ_HEAD_MAX: largest acceptable request head (request-line + headers + CRLFCRLF).
   - BODY_MAX:     hard cap on Content-Length we accept (defends against DoS).
   - SEND_HEAD_MAX:largest serialized status-line + headers we will produce.
*/
#define REQ_HEAD_MAX   16384
#define BODY_MAX       (1 << 20)        /* 1 MiB */
#define SEND_HEAD_MAX  4096

#define SHORT_LENGTH         32
#define HEADERS_LEN          32
#define HEADER_NAME_LENGTH   64
#define HEADER_VALUE_LENGTH  512
#define URI_LENGTH           512
#define VERSION_LENGTH       16
#define REASON_LENGTH        64

/*
   File descriptors are 'int' on POSIX. The earlier int8_t typedef silently
   broke on any fd above 127 (and on errors that return -1 from accept()
   it accidentally compared correctly only by luck).
*/
typedef int      socket_fd_t;
typedef int      success_flag_t;
typedef ssize_t  n_bytes_t;


typedef struct {
    char name[HEADER_NAME_LENGTH];
    char value[HEADER_VALUE_LENGTH];
} header_t;

typedef struct {
    char method[SHORT_LENGTH];
    char uri[URI_LENGTH];
    char http_version[VERSION_LENGTH];
} request_line_t;

typedef struct {
    char    http_version[VERSION_LENGTH];
    int16_t status;
    char    reason[REASON_LENGTH];
} status_line_t;

typedef struct {
    header_t headers[HEADERS_LEN];
    size_t   headers_counter;
} headers_list_t;

typedef struct {
    request_line_t* req_line;
    headers_list_t* head;
    char*           body;
    size_t          body_size;
} http_request_t;

typedef struct {
    status_line_t*  res_line;
    headers_list_t* head;
    char*           body;
    size_t          body_size;
} http_response_t;


#endif
