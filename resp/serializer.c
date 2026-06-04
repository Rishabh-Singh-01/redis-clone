#include "./serializer.h"

void init_serializer(Serializer *serializer) {
  serializer->buffer_itr_idx = 0;
  serializer->buffer_size = 100;
  serializer->buffer = (char *)malloc(sizeof(char) * serializer->buffer_size);
  memset(serializer->buffer, '\0', sizeof(char) * 100);
}

void cleanup_serializer(Serializer *serializer) {
  serializer->buffer_itr_idx = 0;
  serializer->buffer_size = 100;
  free(serializer->buffer);
}

void init_response(Response *response) {
  response->type = Res_Null;
  response->argc = 0;
  response->args = NULL;
}

void cleanup_response(Response *response) { free(response->args); }

void send_response_only(int client_fd, Response *response,
                        Serializer *serializer) {
  if (response->type == Res_Simple_String) {
    sprintf(serializer->buffer, "+%s\r\n", response->args[0]);
  } else if (response->type == Res_Integer) {
    sprintf(serializer->buffer, ":%d\r\n", atoi(response->args[0]));
  } else if (response->type == Res_Bulk_String) {
    sprintf(serializer->buffer, "$%d\r\n%s\r\n", (int)strlen(response->args[0]),
            response->args[0]);
  } else {
  }
  int byte_sent =
      send(client_fd, serializer->buffer, strlen(serializer->buffer), 0);
  if (byte_sent < 0) {
    perror("Error: Failed to write to client");
  }
}
