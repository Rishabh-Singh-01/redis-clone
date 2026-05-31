#ifndef COMMON_HEADERS
#define COMMON_HEADERS
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
typedef enum Command_Type_Enum {
  Cmd_Ping,
  Cmd_Echo,
  Cmd_Get,
  Cmd_Set,
  Cmd_Del,
  Cmd_Expire,
  Cmd_TTL,
  Cmd_Invalid
} Command_Type;

typedef struct Request {
  int req_split_count;
  Command_Type req_command_type;
  int req_arg_size;
  int req_arg_count;
  int req_arg_idx;
  char *req_args;
} Request;

typedef struct Response {
  Command_Type res_command_type;
  int res_size;
  int result_size;
  char *result;
} Response;

typedef struct TCP_Server {
  int socket_fd;
  int concurrent_conn;
  int buffer_size;
  char *buffer;
} TCP_Server;

extern char CRLF[2];

#endif // !COMMON_HEADERS
//
