#include "../include/http.h"


success_flag_t request_init(http_request_t *req, 
                            char* method, 
                            char* uri, 
                            header_t headers[HEADERS_LEN], 
                            size_t headers_count,
                            char* body) 
{
    //allocating header.
    headers_list_t* hd = NULL;
    request_line_t* rl = NULL;
    req->body = NULL;

    if(headers_count > HEADERS_LEN) goto cleanup;

    hd = malloc(sizeof(header_t) * HEADERS_LEN);
    if(!hd)
    {   
        perror("malloc headers at request_init()@http.c");
        goto cleanup;
    }
    for(size_t i = 0 ; i < headers_count; i++)
    {
        strncpy(hd->headers[i].name, headers[i].name, HEADER_NAME_LENGTH - 1);
        hd->headers[i].name[HEADER_NAME_LENGTH - 1] = '\0';

        strncpy(hd->headers[i].value, headers[i].value, HEADER_NAME_LENGTH - 1);
        hd->headers[i].value[HEADER_NAME_LENGTH - 1] = '\0';
    }
    hd->headers_counter = headers_count;
    
    //allocating request_line

    rl = malloc(sizeof(request_line_t));
    if(!rl)
    {
        perror("malloc rl at request_init()@http.c");
        goto cleanup;
    }
    strcpy(rl->method, method);
    strcpy(rl->uri, uri);
    strcpy(rl->http_version, "HTTP/1.0"); //will be extended later to include HTTP/1.1.

    

    if(body)
    {
        req->body = malloc(strlen(body) + 1);
        if(!req->body) 
        {   
            perror("malloc body at request_init()@http.c");
            goto cleanup;
        }
        strcpy(req->body, body);
        req->body_size = strlen(body);
    }
    else
    {
        req->body = NULL;
        req->body_size = 0;
    }
    
    req->req_line = rl;
    req->head = hd;
    return SUCCESS;


    cleanup:
        perror("Error  at request_init()@http.c\n");
        free(rl);
        free(hd);
        free(req->body);
        return FAILURE;
}





