
#include <stdint.h>

#ifndef COMMON_H
#define COMMON_H


#define FAILURE -1
#define SUCCESS 0

#define BUFF_SIZE 4096
#define RECV_BUFFER_SIZE 8192

#define SHORT_LENGTH 32
#define HEADERS_LEN 32
#define HEADER_NAME_LENGTH 64
#define HEADER_VALUE_LENGTH 512

#define BODY_SIZE 8192  

typedef int8_t socket_fd_t;
typedef size_t msg_len_t;
typedef int8_t success_flag_t;
typedef int16_t n_bytes_t;


typedef struct {
    char name[HEADER_NAME_LENGTH];
    char value[HEADER_VALUE_LENGTH];
}header_t;



typedef struct
{
    char method[SHORT_LENGTH];
    char uri[SHORT_LENGTH];
    char http_version[SHORT_LENGTH];

} request_line_t;

typedef struct 
{
    header_t headers[HEADERS_LEN];
    size_t headers_counter;
} headers_list_t;

typedef struct 
{
    request_line_t* req_line;
    headers_list_t* head;
    char* body;
    size_t body_size;

}http_request_t;

#endif
