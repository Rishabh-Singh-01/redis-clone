#ifndef RESP_REQ
#define RESP_REQ

#include "./../common/headers.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

void deserialize_request(int client_fd, TCP_Server *tcp_server);

#endif // !RESP_REQ
