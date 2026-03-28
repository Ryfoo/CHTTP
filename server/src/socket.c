#include "../include/socket.h"


void address_init(struct sockaddr_in* addr, char* ip, char* port) {
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(port);
}

socket_fd_t listening_starter(struct sockaddr_in* addr)
{
    socket_fd_t fd;

    fd = socket(AF_INET,SOCK_STREAM, 0);
    if (fd < 0) {
        printf("Error establishing a socket\n");
        return FAILURE;
    }
    if(bind(fd, (struct sockaddr*) addr, sizeof(*addr)) < 0) {
        printf("Error binding (giving the local fd) the address : %s\n", addr->sin_addr);
        close(fd);
        return FAILURE;
    }
    if (listen(fd, QUEUE_LIMIT) < 0) {
        printf("Error commecing the server\n");
        close(fd);
        return FAILURE;
    }
    
    return fd;
}


void monitor(socket_fd_t fd, struct sockaddr_in* addr)
{
    while(CONNECTION) {
        socklen_t len = sizeof(struct sockaddr_in);
        socket_fd_t new_fd = accept(fd, (struct sockaddr *) addr, &len);
        char* buffer = NULL;
        if(new_fd < 0) {
            perror("Error establishing a file descriptor for a new connection\n");
            continue;
        }
        printf("The connection has been established\n");
        char* buffer = malloc(RECV_BUFFER_SIZE);
        if(!buffer){
            perror("Error allocating memory\n");
            goto cleanup;
        }

        memset(buffer, 0, RECV_BUFFER_SIZE);
        strcpy(buffer, "hello mate, welcome to the team\n");
        if (send(new_fd, buffer, RECV_BUFFER_SIZE, 0) < 0) {
            perror("Error sending data from the server\n");
            goto cleanup;
        }

        memset(buffer, 0, RECV_BUFFER_SIZE);
        if (recv(new_fd, buffer, RECV_BUFFER_SIZE, 0) < 0) {
            perror("Error sending data\n");
            goto cleanup;
        }
        if(handle_http_request(buffer) < 0) {
            perror("Error handling the request\n");
            goto cleanup;
        }
        cleanup:
            free(buffer);
            close(new_fd);
    }
}



/*
    Mirrors listening_starter() but for the client.
    No bind(), no listen() — just socket() + connect().
*/
socket_fd_t connection_starter(struct sockaddr_in *addr) {
    socket_fd_t fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("Error creating socket\n");
        goto cleanup;
    }

    if (connect(fd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
        printf("Error connecting to server\n");
        goto cleanup;
    }

    return fd;
    cleanup:
        close(fd);
        return FAILURE;
}


n_bytes_t exchange(    
                    int fd, 
                    struct sockaddr_in *addr, 
                    char* method, 
                    char* uri, 
                    header_t headers[HEADERS_LEN],
                    size_t headers_count,
                    char* body
                )
{
    char* buffer = malloc(RECV_BUFFER_SIZE);
    if(!buffer) {
        perror("Error allocating memory (Client's Side)\n");
        goto cleanup;
    }
    n_bytes_t bytes;

    // Demo, here where the serialization should take place.
    // http_req instance -> serialize to shareable data -> send
    http_request_t req;
    if(request_init(&req, 
                    method, 
                    uri, 
                    headers,
                    headers_count,
                    body) != SUCCESS
                ) 
    {
        perror("Error creating an HTTP request\n");
        goto cleanup;
    }
    if(serialize(   req, 
                    buffer, 
                    RECV_BUFFER_SIZE
                    ) != SUCCESS
                )
    {
        goto cleanup;
    }
    // if (send(fd, request, strlen(request), 0) < 0) {
    //     perror("Error sending request\n");
    //     return FAILURE;
    // }

    /*
        Wait for the server response.
        data -> parse to a string/json -> instance of http_res
    */
    memset(buffer, 0, RECV_BUFFER_SIZE);
    bytes = recv(fd, buffer, RECV_BUFFER_SIZE, 0);
    if (bytes < 0) {
        perror("Error receiving response\n");
        goto cleanup;
    }


    //here where the showcasing of the response should take place.
    printf("Server response:\n%s\n", buffer);
    free(buffer);
    return bytes;

    cleanup:
        free(buffer);
        perror("the client failed to connect\n");
        return FAILURE;
}