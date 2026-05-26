#ifndef RESP_DESERIALIZER
#define RESP_DESERIALIZER

#include "./command.h"
#include "./req.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void initialize_request(Request *request);
void reset_request(Request *request);
void initialize_deserializer(Deserializer *deserializer);
void execute_deserializer(Request *request, Deserializer *deserializer);

#endif // !
