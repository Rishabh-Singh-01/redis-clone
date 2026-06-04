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

typedef enum Response_Type_Enum {
  Res_Simple_String,
  Res_Bulk_String,
  Res_Integer,
  Res_Null,
  Res_Invalid,
} Response_Type;

typedef struct Command_Struct {
  Command_Type command_type;
  int argc;
  char **args;
} Command;

typedef struct Response {
  Response_Type type;
  int argc;
  char **args;
} Response;

typedef struct TCP_Server {
  int socket_fd;
  int concurrent_conn;
  int buffer_size;
  char *buffer;
} TCP_Server;

extern char CRLF[2];

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
