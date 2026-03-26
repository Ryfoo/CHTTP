#include "../include/handler.h"


n_bytes_t http_handler(char* buffer)
{
    n_bytes_t bytes;
    http_request_t* req = serialize(buffer);
    success_flag_t i = http_request_handler();
    
    http_response_t* res = http_response_handler();

    return bytes;
}

