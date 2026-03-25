//socket.h
//this header file provides wrappers for the pre-existing socket library
//for better error handling, and additionl features, you want an example ? 
//retransmission for instance.

#ifndef SOCKET_H
#define SOCKET_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

// user-defined constants
// address options
// the file descriptor, the essance of everything, in this program


#define QUEUE_LIMIT 1000
#define CONNECTION 1
extern struct sockaddr_in addr;
int fd;


//the server's engine
int server_engine(int d, int t, int p);


#endif