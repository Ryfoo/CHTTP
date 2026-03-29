#include "../include/server.h"

/*
    Constants offered by socket.h
    AF_INET (IP/v4), SOCK_STREAM (type of connection)

*/


success_flag_t server_engine(char* ip, char* port, int8_t running) {
    struct sockaddr_in addr;
    int fd;
    
    /*
        initializing and preparting  method[SHORT_LENGTH];the struct sockaddr with 
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
        return FAILURE;
    }
    /*
        Here, lies the heart of the server.
        an active loop to recieve requests and handling them later.
        a common file descriptor will be created, to send and recieve data.
    
    */

    monitor(fd, &addr, running);
    

    
    close(fd);
    return SUCCESS;
}

