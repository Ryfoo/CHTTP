#include "socket.h"


struct sockaddr* address_init(struct sockaddr_in* addr, char* ip, int port) {
    memset(&addr, 0, sizeof(addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(port);
    return (struct sockaddr*) addr;
}

int connection_starter()
{
    int fd;

    fd = socket(AF_INET,SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }
    if(!bind(fd, (struct sockaddr*) &addr, sizeof(addr))) {
        return -1;
    }
    if (!listen(fd, QUEUE_LIMIT)) {
        return -1;
    }
    
    return fd;
}