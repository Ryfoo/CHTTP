/*
    server.h
    this header file provides wrappers for the pre-existing socket library
    for better error handling, and additionl features, you want an example ? 
    retransmission for instance.
*/
#pragma once

#ifndef SOCKET_H
#define SOCKET_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/handler.h"
#include "../include/common.h"

// user-defined constants

#define QUEUE_LIMIT 1000
#define CONNECTION 1

/*
 * takes a pointer to sockaddr_in as an argument, consider the manual for more details.
 * initializes the entire memory region occupied by the struct sockaddr_in to zero. Using memset().
 * follows the options AF_INET, SOCK_STREAM (seek more details in the manual page of socket).
 * to set the options according to the IPv4 protocol.
 */


void address_init(struct sockaddr_in* addr, char* ip, char* port);
/*
 * takes a pointer to sockaddr_in as an argument, starts the listening on a file descriptor.
 * that should be returned.
 * initilizes file-descriptor -> binds -> listens.
 * follows the parameters AF_INET, SOCK_STREAM (man socket for more details).
 * return -1 on failure, 0 on success.
 */
int listening_starter(struct sockaddr_in* addr);

/*
 * takes as arguments int file descriptor and an address of type sockaddr (consider the manual page of accept()).
 * active loop to receive requests. 
 * follows the parameters AF_INET, SOCK_STREAM (man socket for more details).
 * return -1 on failure, 0 on success.
 */

success_flag_t monitor(socket_fd_t fd, struct sockaddr_in* addr);


// typedef int socket_fd_t;
// typedef struct http_request http_request_t;
// typedef struct http_response http_response_t;
// typedef int n_bytes_t


#endif