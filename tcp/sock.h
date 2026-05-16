#ifndef TCP_SOCK
#define TCP_SOCK
#include "./../common/headers.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct TCP_Server {
  int socket_fd;
  int concurrent_conn;
  int buffer_size;
  char *buffer;
} TCP_Server;

int start_tcp_server(int port);

#endif /* ifndef TCP_SOCK */
