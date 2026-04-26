#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "server/include/server.h"
#include "server/include/common.h"


static volatile int g_running = 1;

static void on_signal(int sig)
{
    (void)sig;
    g_running = 0;
}

int main(int argc, char* argv[])
{
    /*
       Usage:
         driver <port>            -> binds 0.0.0.0:<port>  (LAN-reachable)
         driver <ip> <port>       -> binds <ip>:<port>     (use 0.0.0.0 / *
                                                            for every interface,
                                                            127.0.0.1 for local-only)
    */
    const char* ip;
    const char* port;
    if (argc == 2) {
        ip   = "0.0.0.0";
        port = argv[1];
    } else if (argc == 3) {
        ip   = argv[1];
        port = argv[2];
    } else {
        fprintf(stderr,
                "Usage:\n"
                "  %s <port>          (binds 0.0.0.0, reachable from LAN)\n"
                "  %s <ip> <port>     (use 127.0.0.1 for local-only)\n",
                argv[0], argv[0]);
        return FAILURE;
    }

    struct sigaction sa = { .sa_handler = on_signal };
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    fprintf(stdout, "scratchy/1.1 binding %s:%s\n", ip, port);

    if (server_engine(ip, port, &g_running) != SUCCESS) {
        fprintf(stderr, "server crashed\n");
        return FAILURE;
    }
    return SUCCESS;
}
