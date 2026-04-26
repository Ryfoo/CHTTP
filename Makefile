# ===== Scratchy HTTP/1.1 server =====

CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -g -D_GNU_SOURCE -Iserver/include -pthread
LDFLAGS = -pthread

SERVER_SRCS = driver.c \
              server/src/server.c \
              server/src/socket.c \
              server/src/http.c \
              server/src/parser.c \
              server/src/serializer.c \
              server/src/router.c \
              server/src/handler.c \
              server/src/mime.c \
              server/src/utils.c

CLIENT_SRCS = server/client_driver.c \
              server/src/client.c \
              server/src/socket.c \
              server/src/http.c \
              server/src/parser.c \
              server/src/serializer.c \
              server/src/router.c \
              server/src/handler.c \
              server/src/mime.c \
              server/src/utils.c

TARGET        = driver
CLIENT_TARGET = client

all: $(TARGET)

$(TARGET): $(SERVER_SRCS)
	$(CC) $(CFLAGS) $(SERVER_SRCS) -o $(TARGET) $(LDFLAGS)

client: $(CLIENT_SRCS)
	$(CC) $(CFLAGS) $(CLIENT_SRCS) -o $(CLIENT_TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(CLIENT_TARGET)

.PHONY: all client clean
