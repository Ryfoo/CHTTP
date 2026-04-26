#include "../include/server.h"
#include "../include/socket.h"
#include <stdio.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


/* Print every IPv4 address bound to a non-loopback interface, plus
   loopback. Helps the user know which URL works from a phone vs the host. */
static void print_lan_addresses(const char* port)
{
    struct ifaddrs* ifs = NULL;
    if (getifaddrs(&ifs) != 0) return;

    fprintf(stdout, "reachable URLs:\n");
    for (struct ifaddrs* it = ifs; it; it = it->ifa_next) {
        if (!it->ifa_addr) continue;
        if (it->ifa_addr->sa_family != AF_INET) continue;

        struct sockaddr_in* sa = (struct sockaddr_in*)it->ifa_addr;
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sa->sin_addr, buf, sizeof(buf));

        const char* tag = "lan";
        if (strcmp(buf, "127.0.0.1") == 0) tag = "loopback";
        fprintf(stdout, "  http://%s:%s   (%s, %s)\n",
                buf, port, it->ifa_name, tag);
    }
    freeifaddrs(ifs);
}


success_flag_t server_engine(const char* ip, const char* port,
                             volatile int* running)
{
    struct sockaddr_in addr;
    address_init_ip(&addr, ip, port);

    socket_fd_t fd = listening_starter(&addr);
    if (fd < 0) return FAILURE;

    print_lan_addresses(port);
    fprintf(stdout,
            "if a phone on the same Wi-Fi can't reach the LAN URL, the host "
            "firewall is blocking inbound TCP %s.\n", port);
    fflush(stdout);

    monitor(fd, running);
    close(fd);
    return SUCCESS;
}
