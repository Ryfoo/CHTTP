#include "../include/router.h"


success_flag_t router_handler(http_request_t req, http_request_t* res)
{
    
    if(strcmp(req.req_line->method, "GET"))
    {
        if((req.req_line->uri, "/index.html"))
        {
            return index_handler(req, res);
        }

    }
    // if(strcmp(req.req_line->method, "POST"))

    return make_404_response();
}