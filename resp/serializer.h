#ifndef RESP_SERIALIZER
#define RESP_SERIALIZER

#include "./../common/headers.h"
#include "./req.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct Serialize_Response_State_Struct {
  int buffer_size;
  char *buffer;
  int buffer_itr_idx;
} Serializer;

void cleanup_serializer(Serializer *serializer);
void init_serializer(Serializer *serializer);
void send_response(int client_fd, Request *request, Response *response,
                   Serializer *serializer);

#endif // !RESP_SERIALIZERk
