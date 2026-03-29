#include "../include/client.h"

/*
    Mirrors server_engine() on the client side.
    Instead of bind() -> listen() -> accept(),
    the client only needs socket() -> connect().
*/
success_flag_t client_engine(
                            char* ip, 
                            char* port,
                            char* method,
                            char* uri, 
                            header_t headers[HEADERS_LEN],
                            size_t headers_count,
                            char* body
                        ) 
{
    /*
        Note that sockaddr_in data type is heavily used in that code
        if you can't figure out what type of data it carries,
        it's advised to use the manual
    */
    struct sockaddr_in addr;
    socket_fd_t fd;
    http_response_t* res = NULL;
    /*
        use address_init() from socket.h
    */
    address_init(&addr, ip, port);

    /*
        connection_starter() is the client equivalent of listening_starter().
        calls socket() with IPv4 parameters
        then calls connect() to reach the remote server.
    */
    fd = connection_starter(&addr);
    if (fd < 0) {
        perror("Error establishing connection\n");
        free(res);
        return FAILURE;
    }

    /*
        Heart of the client — symmetric to monitor() on the server.
        sends requests and receives responses
        using the server's address.
    */
    res = malloc(sizeof(http_response_t));
    if(!res){
        goto cleanup;
    }
    if (exchange(
                    fd, 
                    &addr, 
                    method, 
                    uri, 
                    headers,
                    headers_count,
                    body,
                    res   
                ) < 0) {
        printf("Error sending data to the server\n");
        goto cleanup;
    }

    printf("this is the content :\n %s \n", res->body);
    free(res->res_line);
    free(res->head);
    free(res->body);
    free(res);
    close(fd);
    return SUCCESS;
    cleanup:
        free(res);
        close(fd);
        return FAILURE;
}