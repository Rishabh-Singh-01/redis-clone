#ifndef COMMON_HEADERS
#define COMMON_HEADERS
#include <assert.h>
#include <stdio.h>
typedef enum RequestType {
  Req_Ping,
} RequestType;

typedef struct Request {
  int args;
  RequestType *requestType;
} Request;

typedef struct TCP_Server {
  int socket_fd;
  int concurrent_conn;
  int buffer_size;
  char *buffer;
} TCP_Server;

#endif // !COMMON_HEADERS
