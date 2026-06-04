#ifndef COMMON_HEADERS
#define COMMON_HEADERS

#include "stdlib.h"
#include <arpa/inet.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "./utils.h"

typedef enum Command_Type_Enum {
  Cmd_Ping,
  Cmd_Echo,
  Cmd_Get,
  Cmd_Set,
  Cmd_Del,
  Cmd_Expire,
  Cmd_TTL,
  CmdI_ExpiresAt,
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

typedef struct Command_Struct {
  Command_Type command_type;
  int argc;
  char **args;
} Command;

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

typedef struct Deserialize_Request_State_Struct {
  int bytes_read_count;
  int read_buffer_offset_idx;

  int crlf_visited_count;
  int cur_arg_consumed_count;
  int cur_arg_size;
  int buffer_size;
  char *buffer;
  int buffer_itr_idx;
} Deserializer;

typedef struct Req_Parser {
  int buffer_size;
  char *buffer;
} Req_Parser;

typedef struct STORAGE_HASHMAP_INTERNAL_LL_STRUCT {
  char *key;
  char *value;
  time_t expires_at;
  struct STORAGE_HASHMAP_INTERNAL_LL_STRUCT *next;
} Storage_hashmap_internal_ll;

typedef struct STORAGE_HASHMAP_STRUCT {
  int buckets_count;
  Storage_hashmap_internal_ll **bucket;
} Storage_hashmap;

typedef struct Serialize_Response_State_Struct {
  int buffer_size;
  char *buffer;
  int buffer_itr_idx;
} Serializer;

#endif // !COMMON_HEADERS
//
