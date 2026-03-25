#include "socket.h"

// constants offer by socket.h : AF_INET (IP/v4), SOCK_STREAM (type of connection)


int server_engine(int d, int t, int p) {
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8080);

    fd = socket(AF_INET,SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }
    if(!bind(fd, (struct sockaddr*) &addr, sizeof(addr))) {
        return -1;
    }
    if (!listen(fd, QUEUE_LIMIT)) {
        return -1;
    }
    while(CONNECTION) {
        if(!accept()) {
            return -1;
        }
    }
    return 0;
}