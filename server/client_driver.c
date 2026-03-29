#include <stdio.h>
#include <stdlib.h>
#include "include/utils.h"
#include "include/client.h"
#include "include/common.h"

int main(int args, char* argvs[]) {
    load_env(".env");
    char* address = getenv("IP");
    char* port = getenv("PORT");
    printf("trying to connect to the server at %s:%s\n", address, port);


    header_t headers[HEADERS_LEN];
    headers[0] = (header_t){
        .name = "Content-type",
        .value = "text/html"
    };


    if(client_engine(
                        "127.0.0.1", 
                        "8080",
                        "GET",
                        "/index.html",
                        headers,
                        1,
                        ""
                    ) < 0) {    
        printf("client disconnected!\n");
        return FAILURE;
    }
    return SUCCESS;
}