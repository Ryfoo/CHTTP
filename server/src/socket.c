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


success_flag_t monitor(socket_fd_t fd, struct sockaddr_in* addr)
{
    while(CONNECTION) {
        socklen_t len = sizeof(struct sockaddr_in);
        socket_fd_t shared_fd = accept(fd, (struct sockaddr *) addr, &len);
        if(shared_fd < 0) {
            perror("Error establishing a common file descriptor\n");
            continue;
        }
        printf("The connection has been established\n");
        char* buffer = malloc(BUFF_SIZE);
        if(!buffer){
            perror("Error allocating memory\n");
            close(shared_fd);
            continue;
        }
        memset(buffer, 0, BUFF_SIZE);
        buffer = strcpy(buffer, "hello mate, welcome to the team\n");
        msg_len_t sent = send(shared_fd, buffer, BUFF_SIZE, 0);
        if (sent < 0) {
            perror("Error sending data over the common file descriptor\n");
            free(buffer);
            close(shared_fd);
            continue;
        }
        memset(buffer, 0, BUFF_SIZE);
        msg_len_t received = recv(shared_fd, buffer, BUFF_SIZE, 0);
        if (sent < 0) {
            perror("Error sending data over the common file descriptor\n");
            free(buffer);
            close(shared_fd);
            continue;
        }
        success_flag_t s = handle_http_request(buffer);
        if(s < 0) {
            perror("Error handling the request\n");
            free(buffer);
            close(shared_fd);
            return FAILURE;
        }
        free(buffer);
        close(shared_fd);
    }
    return 0;
}



/*
    Mirrors listening_starter() but for the client.
    No bind(), no listen() — just socket() + connect().
*/
socket_fd_t connection_starter(struct sockaddr_in *addr) {
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("Error creating socket\n");
        return FAILURE;
    }

    if (connect(fd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
        printf("Error connecting to server\n");
        close(fd);
        return FAILURE;
    }

    return fd;
}


n_bytes_t exchange(    
                            int fd, 
                            struct sockaddr_in *addr, 
                            char* method, 
                            char* uri, 
                            header headers[HEADERS_LEN],
                            char* body
                        )
{
    char* buffer = malloc(BUFF_SIZE);
    if(!buffer) {
        perror("Error allocating memory (Client's Side)\n");
        return FAILURE;
    }
    n_bytes_t bytes;

    // Demo, here where the serialization should take place.
    // http_req instance -> serialize to shareable data -> send
    http_request_t req;
    if(request_constructor(&req, method, uri, headers, body) < 0) 
    {
        perror("Error creating an HTTP request\n");
        return FAILURE;
    }
    if(serialize(req, buffer, BUFF_SIZE) < 0)
    {
        return FAILURE;
    }
    // if (send(fd, request, strlen(request), 0) < 0) {
    //     perror("Error sending request\n");
    //     return FAILURE;
    // }

    /*
        Wait for the server response.
        data -> parse to a string/json -> instance of http_res
    */
    memset(buffer, 0, BUFF_SIZE);
    bytes = recv(fd, buffer, BUFF_SIZE, 0);
    if (bytes < 0) {
        perror("Error receiving response\n");
        return FAILURE;
    }


    //here where the showcasing of the response should take place.
    printf("Server response:\n%s\n", buffer);
    return bytes;
}