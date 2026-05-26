#include "./serializer.h"

void init_serializer(Serializer *serializer) {
  serializer->buffer_itr_idx = 0;
  serializer->buffer_size = 100;
  serializer->buffer = (char *)malloc(sizeof(char) * serializer->buffer_size);
  memset(serializer->buffer, '\0', sizeof(char) * 100);
}

void send_response(int client_fd, Request *request, Serializer *serializer) {
  if (request->req_command_type == Req_Ping) {
    memcpy(serializer->buffer, "+PONG\r\n", strlen("+PONG\r\n"));

  } else if (request->req_command_type == Req_Echo &&
             request->req_arg_count != 2) {
    char *err_msg = "-ERR wrong number of arguments for 'echo' command\r\n";
    memcpy(serializer->buffer, err_msg, strlen(err_msg));

  } else if (request->req_command_type == Req_Echo) {
    char *first_arg_idx = request->req_args + (request->req_arg_size * 1);
    int first_arg_size = strlen(first_arg_idx);
    serializer->buffer[serializer->buffer_itr_idx++] = '$';
    serializer->buffer[serializer->buffer_itr_idx++] = first_arg_size + '0';
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
    char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
    memcpy(buf_cp_idx, first_arg_idx, first_arg_size);
    serializer->buffer_itr_idx += first_arg_size;
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];

  } else {
    char *err_msg = "-ERR unknown command\r\n";
    memcpy(serializer->buffer, err_msg, strlen(err_msg));
  }
  int byte_sent =
      send(client_fd, serializer->buffer, strlen(serializer->buffer), 0);
  if (byte_sent < 0) {
    perror("Error: Failed to write to client");
  }
}
