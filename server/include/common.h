
#ifndef TYPES_H
#define TYPES_H


#define FAILURE -1
#define SUCCESS 0

#define BUFF_SIZE 4096
#define RECV_BUFFER_SIZE    8192

#define HEADERS_LEN 32
#define HEADER_NAME_LENGTH 64
#define HEADER_VALUE_LENGTH 512

#define BODY_SIZE 8192  

typedef int8_t socket_fd_t;
typedef ssize_t msg_len_t;
typedef int8_t success_flag_t;
typedef int16_t n_bytes_t;


typedef struct {
    char name[32];
    char value[256];
}header;

typedef struct 
{
    struct request_line_t* req_line;
    struct headers_t* headers;
    char* body;
    size_t body_size;

}http_request_t;

typedef struct
{
    char* method;
    char* uri;
    char* http_version;

} request_line_t;

typedef struct 
{
    header headers[HEADERS_LEN];
    size_t headers_counter;
} headers_t;



void http_request_init(http_request_t *req);
void http_request_add_header(http_request_t *req, const char *name, const char *value);
void http_request_set_body(http_request_t *req, const char *data, size_t len);

#endif
