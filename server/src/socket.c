#include "socket.h"


void address_init(struct sockaddr_in* addr, char* ip, int port) {
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(port);
}

int listening_starter(struct sockaddr_in* addr)
{
    int fd;

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


int monitor(int fd, struct sockaddr* addr)
{
    int shared_fd;
    while(CONNECTION) {
        socklen_t len = sizeof(struct sockaddr_in);
        shared_fd = accept(fd, addr, &len);
        char *buffer;
        if(shared_fd < 0) {
            perror("Error establishing a common file descriptor\n");
            continue;
        }
        printf("The connection has been established\n");
        buffer = malloc(BUFF_SIZE * sizeof(char));
        if(!buffer){
            perror("Error allocating memory\n");
            close(shared_fd);
            continue;
        }
        memset(buffer, 0, BUFF_SIZE * sizeof(char));
        buffer = strcpy(buffer, "hello mate, welcome to the team\n");
        if (send(shared_fd, buffer, BUFF_SIZE, 0) < 0) {
            perror("Error sending data over the common file descriptor\n");
            free(buffer);
            close(shared_fd);
            continue;
        }
        free(buffer);
        close(shared_fd);
    }
    
    close(shared_fd);
    return shared_fd;
}