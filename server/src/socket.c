#include "../include/socket.h"


void address_init(struct sockaddr_in* addr, char* ip, char* port) {
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr((const char*)ip);
    addr->sin_port = (in_port_t) htons((u_int16_t)atoi((const char*)port));
}

socket_fd_t listening_starter(struct sockaddr_in* addr)
{
    socket_fd_t host_fd;

    host_fd = socket(AF_INET,SOCK_STREAM, 0);
    if (host_fd < 0) {
        printf("Error establishing a socket\n");
        return FAILURE;
    }
    if(bind(host_fd, (struct sockaddr*) addr, sizeof(*addr)) < 0) {
        perror("Error binding (giving the local fd) the address\n");
        close(host_fd);
        return FAILURE;
    }
    if (listen(host_fd, QUEUE_LIMIT) < 0) {
        printf("Error commecing the server\n");
        close(host_fd);
        return FAILURE;
    }
    
    return host_fd;
}


void monitor(socket_fd_t host_fd, struct sockaddr_in* addr, int8_t running)
{
    while(running) {
        socklen_t len = sizeof(struct sockaddr_in);
        socket_fd_t transfer_fd = accept(host_fd, (struct sockaddr *) addr, &len);
        if(transfer_fd < 0) {
            perror("Error establishing connection\n");
            continue;
        }
        char* send_buffer = NULL;
        char* recv_buffer = NULL;
        http_request_t* req = NULL;
        http_response_t* res = NULL;

        printf("The connection has been established\n");


        send_buffer = malloc(SEND_BUFF_SIZE);
        recv_buffer = malloc(RECV_BUFF_SIZE);
        if(!send_buffer || !recv_buffer){
            perror("Error allocating memory\n");
            goto cleanup;
        }


        // waiting for the HTTP request.
        memset(recv_buffer, 0, RECV_BUFF_SIZE);
        if (recv(transfer_fd, recv_buffer, RECV_BUFF_SIZE, 0) < 0) {
            perror("Error sending data\n");
            goto cleanup;
        }
        
        req = malloc(sizeof(http_request_t));
        res = malloc(sizeof(http_response_t));

        if(handle_http_request(recv_buffer, send_buffer, req, res) != SUCCESS) {
            perror("Error handling the request\n");
            goto cleanup;
        }
        if(send(transfer_fd, send_buffer, SEND_BUFF_SIZE, 0) < 0)
        {
            perror("Error sending data\n");
            goto cleanup;
        }
        free(recv_buffer);
        free(send_buffer);
        

        free(req->req_line);
        free(req->head);
        free(req->body);
        free(req);


        free(res->res_line);
        free(res->head);
        free(res->body);
        free(res);


        close(transfer_fd);
        
        cleanup:
            free(recv_buffer);
            free(send_buffer);

            free(req->req_line);
            free(req->head);
            free(req->body);
            free(req);
            

            free(res->res_line);
            free(res->head);
            free(res->body);
            free(res);
            


            close(transfer_fd);
    }
}



/*
    Mirrors listening_starter() but for the client.
    No bind(), no listen() — just socket() + connect().
*/
socket_fd_t connection_starter(const struct sockaddr_in *addr) {
    if(!addr) return (socket_fd_t) FAILURE;
    socket_fd_t conn_fd;

    conn_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn_fd < 0) {
        printf("Error creating socket\n");
        
    }

    if (connect(conn_fd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
        printf("Error connecting to server\n");
        close(conn_fd);
        return (socket_fd_t) FAILURE;
    }

    return conn_fd;
        
}


n_bytes_t exchange(    
                    const socket_fd_t fd, 
                    const struct sockaddr_in *addr, 
                    const char* method, 
                    const char* uri, 
                    header_t headers[HEADERS_LEN],
                    size_t headers_count,
                    const char* body,
                    http_response_t* res
                )
{


    if (fd < 0 || !addr || !method || !uri || !headers) return (n_bytes_t) FAILURE;
    // declaring as null, so the cleanup process can be unified.
    char* send_buffer = NULL;
    char* recv_buffer = NULL;
    http_request_t* req = NULL;


    // allocating memory for data to be sent/recieved.
    send_buffer = malloc(SEND_BUFF_SIZE);
    recv_buffer = malloc(RECV_BUFF_SIZE);
    if(!send_buffer || !recv_buffer) {
        perror("Error allocating memory (Client's Side)\n");
        goto cleanup;
    }
    n_bytes_t bytes;

    // here where the serialization should take place.
    // http_req instance -> serialize to shareable data -> send.
    req = malloc(sizeof(http_request_t));
    if(request_init(
                    req, 
                    method, 
                    uri, 
                    headers,
                    headers_count,
                    body) != SUCCESS
                ) 
    {
        perror("Error creating an HTTP request\n");
        goto cleanup;
    }
    if(serialize_req(send_buffer, req, SEND_BUFF_SIZE) != SUCCESS)   
    {
        goto cleanup;
    }
    if (send(fd, send_buffer, SEND_BUFF_SIZE, 0) < 0) {
        perror("Error sending request\n");
        goto cleanup;
    }

    /*
        Wait for the server response.
        data -> parse to a string/json -> instance of http_res
    */
    memset(recv_buffer, 0, RECV_BUFF_SIZE);
    bytes = recv(fd, recv_buffer, RECV_BUFF_SIZE, 0);
    if (bytes < 0) {
        perror("Error receiving response\n");
        goto cleanup;
    }

    if(parse_res((const char*) recv_buffer, res) != SUCCESS)
    {
        perror("Error parsing response\n");
        goto cleanup;
    }


    
    free(send_buffer);
    free(recv_buffer);
    free(req);
    close(fd);
    return bytes;

    cleanup:
        free(send_buffer);
        free(recv_buffer);
        free(req);
        perror("the client failed to connect\n");
        return (n_bytes_t) FAILURE;
}