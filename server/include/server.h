/*
    server.h
    this header file provides the main beating heart of the server.
    a function that takes two strings: the ip and the port, in order to 
    establish sockets and launch a listening operation on a specified file descriptor
    types are defined at server/include/common.h
*/
#pragma once

#ifndef SERVER_H
#define SERVER_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/socket.h"
#include "../include/common.h"

// user-defined constants


/*
 * takes char* ip and int port as arguments.
 * initializes the server on ip:port.
 * provides error handling
 * returns -1 on failure, 0 on success
 */
success_flag_t server_engine(char* ip, char* port, int8_t running);

#endif