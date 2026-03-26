//server.h
//this header file provides wrappers for the pre-existing socket library
//for better error handling, and additionl features, you want an example ? 
//retransmission for instance.


#ifndef SOCKET_H
#define SOCKET_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>


#define QUEUE_LIMIT 1000
#define CONNECTION 1
#define BUFF 1024
extern struct sockaddr_in addr;
int fd;

struct sockaddr* address_init(struct sockaddr_in* addr);
/*
 * takes no arguments, starts the listening on a file descriptor.
 * that should be returned.
 * initilizes file-descriptor -> binds -> listens.
 * follows the parameters AF_INET, SOCK_STREAM (man socket for more details).
 * return -1 on failure, 0 on success.
 */
int listening_starter();
#endif