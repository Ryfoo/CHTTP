#ifndef SERVER_H
#define SERVER_H

#include "common.h"

/*
   Initializes the server on `ip`:`port` and starts the accept loop.
   `running` is borrowed; the loop continues until *running is set to 0.
   Returns 0 on clean shutdown, -1 on failure.
*/
success_flag_t server_engine(const char* ip, const char* port,
                             volatile int* running);

#endif
