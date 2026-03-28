#include "../include/handler.h"


success_flag_t handle_http_request(char* buffer)
{
    http_request_t req;
    if(parse(buffer, &req) != SUCCESS)
    {
        
        return FAILURE;
    }
    
    //Routing logic (redirection).
}
