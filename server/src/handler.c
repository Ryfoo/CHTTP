#include "../include/handler.h"
success_flag_t make_404_response(http_response_t* res)
{
    res->res_line->status = 404;
    strcpy(res->res_line->reason, "not found");
    
    return FAILURE;
}
success_flag_t index_handler(http_request_t req, http_response_t* res)
{
    if (!res) return FAILURE;

    FILE* f = fopen("../../static/index.html", "r");
    if (!f)
    {
        return make_404_response(res);
    }

    char* buffer = malloc(RECV_BUFF_SIZE);
    if (!buffer)
    {
        fclose(f);
        return make_response(res);
    }

    size_t bytes_read = fread(buffer, 1, RECV_BUFF_SIZE - 1, f);
    buffer[bytes_read] = '\0';

    fclose(f);

    // Build HTTP response
    res->res_line->status = 200;
    strcpy(res->res_line->reason, "OK");
    res->head->headers[0] = (header_t){
        .name = "Content-type",
        .value = "text/html"
    };
    res->body = buffer;
    res->body_size = bytes_read;

    return SUCCESS;
}