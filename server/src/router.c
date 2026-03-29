#include "../include/router.h"


success_flag_t router_handler(http_request_t* req, http_response_t* res)
{
    
    if(strcmp(req->req_line->method, "GET") == 0)
    {
        if(strcmp(req->req_line->uri, "/") == 0)
        {
            return index_handler(req, res);
        }

    }
    // if(strcmp(req.req_line->method, "POST"))

    return make_404_response(res);
}