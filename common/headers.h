#ifndef COMMON_HEADERS
#define COMMON_HEADERS
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
typedef enum Command_Type_Enum {
  Req_Ping,
  Req_Echo,
  Req_Get,
  Req_Set,
  Req_Invalid
} Command_Type;

typedef struct Request {
  int req_split_count;
  Command_Type req_command_type;
  int req_arg_size;
  int req_arg_count;
  int req_arg_idx;
  char *req_args;
} Request;

typedef struct TCP_Server {
  int socket_fd;
  int concurrent_conn;
  int buffer_size;
  char *buffer;
} TCP_Server;

#endif // !COMMON_HEADERS
