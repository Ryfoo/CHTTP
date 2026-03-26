//socket.h
//this header file provides wrappers for the pre-existing socket library
//for better error handling, and additionl features, you want an example ? 
//retransmission for instance.

#ifndef SERVER_H
#define SERVER_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "socket.h"

// user-defined constants


/*
 * takes char* ip and int port as arguments.
 * initializes the server on ip:port.
 * provides error handling
 * returns -1 on failure, 0 on success
 */
int server_engine(char* ip, int port);


#endif