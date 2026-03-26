#include "server.h"

/*
    Constants offered by socket.h
    AF_INET (IP/v4), SOCK_STREAM (type of connection)

*/


int server_engine(char* ip, int port) {
    struct sockaddr_in addr;
    int fd;
    
    /*
        initializing and preparting the struct sockaddr with 
        the desirable options
        address_init is a user-defind function
        for modulartiy, readability and portability
    */
    address_init(&addr, ip, port);

    /*
        connection starter()
        a 3-stage function 
        calls socket() with parameters specialized
        for IPv4 connection 
        binds to the env variable address:port
        begins the listening operation.
    */

    fd = listening_starter(&addr);

    if (fd < 0) {
        return -1;
    }
    /*
        Here, lies the heart of the server.
        an active loop to recieve requests and handling them later.
        a common file descriptor will be created, to send and recieve data.
    
    */

    if(monitor(fd, &addr) < 0) {
        printf("Error establishing a stable connection to receive requests\n");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}