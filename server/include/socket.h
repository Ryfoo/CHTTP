/*
    server.h
    this header file provides wrappers for the pre-existing socket library
    for better error handling, and additionl features, you want an example ? 
    retransmission for instance.
*/

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
 * takes as an argument : 
 * @params pointer to sockaddr_in addr
 * initializes the entire memory region occupied by the struct sockaddr_in to zero, using memset().
 * initilizes socket -> binds -> listens on fd.
 * follows the parameters AF_INET, SOCK_STREAM (man socket for more details).
 * to set the options according to the IPv4 protocol.
 */
void address_init(struct sockaddr_in* addr, char* ip, char* port);


/*
 * takes as an argument : 
 * @params pointer to sockaddr_in addr
 * starts the listening on a file descriptor to be determined.
 * initilizes socket -> binds -> listens on fd.
 * follows the parameters AF_INET, SOCK_STREAM (man socket for more details).
 * return -1 on failure, fd on success.
 */
socket_fd_t listening_starter(struct sockaddr_in* addr);



/*
 * takes as arguments :
 * @params file descriptor of type socket_fd_t (uint8_t)    
 * @params sockaddr address (consider the manual page of socket.h).
 * active loop to receive requests. 
 * follows the options AF_INET, SOCK_STREAM (man socket for more details).
 * return -1 on failure, 0 on success.
 */
success_flag_t monitor(socket_fd_t fd, struct sockaddr_in* addr);



/*
 * takes as arguments :
 * @params file descriptor of type socket_fd_t (uint8_t)   
 * @params sockaddr address (consider the manual page of socket.h).
 * Client-side connection initiator, calls socket() then connect().
 * follows the options AF_INET, SOCK_STREAM (man socket for more details).
 * return -1 on failure, 0 on success.
 */
success_flag_t connection_starter(struct sockaddr_in *addr);

/*
    
    Symmetric to monitor() on the server.
    sends requests and receives responses.
*/
/*
 * takes as arguments :
 * @params file descriptor of type socket_fd_t (uint8_t)  
 * @params pointer to sockaddr_in addr (consider the manual page of socket.h).
 * Client-side communication loop.
 * follows the options AF_INET, SOCK_STREAM (man socket for more details).
 * return -1 on failure, 0 on success.
 */
success_flag_t exchange(int fd, struct sockaddr_in *addr);




#endif