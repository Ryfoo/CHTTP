#include "server.h"

/*
    Constants offered by socket.h
    AF_INET (IP/v4), SOCK_STREAM (type of connection)

*/


int server_engine(char* ip, int port) {
    
    /*
        initializing and preparting the struct sockaddr with 
        the desirable options
        address_init is a user-defind function
        for modulartiy, readability and portability
    */
    struct sockaddr* ad = address_init(&addr, ip, port);

    /*
        connection starter()
        a 3-stage function 
        calls socket() with parameters specialized
        for IPv4 connection 
        binds to the env variable address:port
        begins the listening operation.
    */

    fd = listening_starter();
    while(CONNECTION) {
        int new_fd = accept(fd, (struct sockaddr*) &addr, sizeof(addr));
        if(new_fd < 0) {
            return -1;
        }
        printf("The connection has been established\n");
        char *buffer = malloc(BUFF * sizeof(int));
        int r = recv(new_fd, BUFF, sizeof(BUFF), 0);
        if (r < 0) {
            return -1;
        }
        printf(BUFF);
    }
    return 0;
}