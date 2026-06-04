#include "sock.h"
#include "./../resp/resp.h"

TCP_Server tcp_server;

void init_tcp_server(TCP_Server *tcp_server) {
  tcp_server->socket_fd = 0;
  tcp_server->concurrent_conn = 0;
  tcp_server->buffer_size = 1024;
  tcp_server->buffer = (char *)malloc(sizeof(char) * tcp_server->buffer_size);
  memset(tcp_server->buffer, 0, tcp_server->buffer_size);
}

/*
 * Creates a new TCP listener,
 * Listens over the provided port
 */
int create_tcp_listener(int port) {
  struct sockaddr_in addr = {0};

  tcp_server.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (tcp_server.socket_fd < 0) {
    perror("Unable to create socket");
    return -1;
  }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  int opt = 1;
  if (setsockopt(tcp_server.socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
                 sizeof(opt)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    return -1;
  }

  int bind_res =
      bind(tcp_server.socket_fd, (struct sockaddr *)&addr, sizeof(addr));
  if (bind_res < 0) {
    perror("Unable to bind the socket");
    return -1;
  }

  listen(tcp_server.socket_fd, 5);
  return 0;
}

/*
 * Accepts a connection and reads over it infinitely,
 * Blocks the provided TCP server
 */
int accept_and_read_conn() {
  int client_fd = accept(tcp_server.socket_fd, NULL, NULL);
  if (client_fd < 0) {
    perror("Error: Unable to accept the connection");
    return -1;
  }
  printf("Info: Connection Accepted. Total Connections: %d\n",
         ++tcp_server.concurrent_conn);

  // execute_resp(client_fd, &tcp_server);
  execute_resp_new(client_fd);
  close(client_fd);

  return 0;
}

/*
 * Starts a TCP Server and works infinitely till closed,
 * Currently, blocking and works with single connection
 */
int start_tcp_server(int port) {
  init_tcp_server(&tcp_server);
  // load_aof();

  if (create_tcp_listener(port) < 0) {
    perror("Error: Unable to create tcp listener");
    return -1;
  }

  while (1) {
    if (accept_and_read_conn() < 0) {
      perror("Error: Unable to accept and read conn");
      return -1;
    }
  }

  return 0;
}
