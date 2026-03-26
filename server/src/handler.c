#include "../include/handler.h"


n_bytes_t handle_client(socket_fd_t fd)
{
    char* buffer = malloc(BUFF_SIZE);
    if(!buffer) {
        perror("Error allocating memory\n");
        close(fd);
        return -1;
    }
    memset(buffer, 0, BUFF_SIZE);
    n_bytes_t bytes;
    bytes = recv(fd, buffer, BUFF_SIZE, 0);
    if (bytes <= 0) {
        perror("Error allocating memory\n");
        free(buffer);
        close(fd);
        return -1;
    }
    http_request_t* req;
    success_flag_t i = http_request_handler();
    http_response_t* res;
    success_flag_t j = http_response_handler();
    return bytes;
}