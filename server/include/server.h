/*
    socket.h
    this header file provides wrappers for the pre-existing socket library
    for better error handling, and additionl features, you want an example ? 
    retransmission for instance.
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
success_flag_t server_engine(string ip, string port);

typedef char* string;
#endif