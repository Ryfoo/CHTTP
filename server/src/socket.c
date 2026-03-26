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
        printf("Error establishing a file descriptor\n");
        return -1;
    }
    if(bind(fd, (struct sockaddr*) addr, sizeof(*addr)) < 0) {
        printf("Error binding (giving the local fd) the address : %s\n", addr->sin_addr);
        close(fd);
        return -1;
    }
    if (listen(fd, QUEUE_LIMIT) < 0) {
        printf("Error commecing the server\n");
        close(fd);
        return -1;
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
        if (send(shared_fd, buffer, BUFF_SIZE, 0) < 0) {
            perror("Error sending data over the common file descriptor\n");
            free(buffer);
            close(shared_fd);
            continue;
        }
        if(handle_client(shared_fd) < 0) {
            perror("Error handling the request\n");
            free(buffer);
            close(shared_fd);
            return -1;
        }
        free(buffer);
        close(shared_fd);
    }
    return 0;
}