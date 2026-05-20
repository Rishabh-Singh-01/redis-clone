#include "./req.h"
#include <arpa/inet.h>
#include <stdio.h>

char CRLF[2] = {'\r', '\n'};

void deserialize_request(int client_fd, TCP_Server *tcp_server) {
  printf("Inside the deserialize_request\n");
  Request request = {};
  request.args = 0;
  request.requestType = (RequestType *)malloc(sizeof(RequestType) * 10);

  int read_count = 0;
  int read_buffer_offset_idx = 0;

  int crlf_visited_count = 0;
  int command_byte_consumed = 0;
  int command_byte_size = 0;
  char *req_bytes = malloc(sizeof(char) * 10);
  memset(req_bytes, '\0', sizeof(char) * 10);

  while ((read_count =
              read(client_fd, tcp_server->buffer + read_buffer_offset_idx,
                   tcp_server->buffer_size - read_buffer_offset_idx)) > 0) {
    int buffer_itr = read_buffer_offset_idx;
    read_buffer_offset_idx += read_count;

    while (buffer_itr < read_buffer_offset_idx - 1) {
      if (crlf_visited_count == 0 &&
          tcp_server->buffer[buffer_itr] == CRLF[0] &&
          tcp_server->buffer[buffer_itr + 1] == CRLF[1]) {

        assert(tcp_server->buffer[buffer_itr - 2] == '*' &&
               "Error: Request should start with Arr Command");
        assert(tcp_server->buffer[buffer_itr - 1] > '0' &&
               "Error: Should have more than 0 args");

        crlf_visited_count++;
        request.args = tcp_server->buffer[buffer_itr - 1] - '0';
        buffer_itr += 2;
        continue;
      }

      if (crlf_visited_count > 0 && crlf_visited_count % 2 != 0 &&
          (tcp_server->buffer[buffer_itr] == CRLF[0] &&
           tcp_server->buffer[buffer_itr + 1] == CRLF[1])) {
        assert(tcp_server->buffer[buffer_itr - 2] == '$' &&
               "Error: Request should start with Arr Command");
        assert(tcp_server->buffer[buffer_itr - 1] > '0' &&
               "Error: Should have more than 0 args");

        crlf_visited_count++;
        command_byte_size = tcp_server->buffer[buffer_itr - 1] - '0';
        memset(req_bytes, '\0', sizeof(char) * 10);
        buffer_itr += 2;
        continue;
      }

      if (crlf_visited_count > 0 && crlf_visited_count % 2 == 0) {
        req_bytes[command_byte_consumed] = tcp_server->buffer[buffer_itr];
        command_byte_consumed++;

        if (command_byte_consumed == command_byte_size) {
          req_bytes[command_byte_consumed + 1] = '\0';
          printf("> %s\n", req_bytes);
          command_byte_consumed = 0;
          command_byte_size = 0;
          buffer_itr += 3;
          crlf_visited_count++;
          continue;
        }
      }
      buffer_itr++;
    }
    int byte_sent = send(client_fd, "+PONG\r\n", 7, 0);
    if (byte_sent < 0) {
      perror("Error: Failed to write to client");
    }
    read_count = 0;
    read_buffer_offset_idx = 0;
    crlf_visited_count = 0;
    command_byte_consumed = 0;
    command_byte_size = 0;
    memset(req_bytes, '\0', sizeof(char) * 10);
  }

  if (read_count == 0) {
    printf("Info: Connection closed\n");
  } else {
    perror("Error: Issue with connectio during close\n");
  }
  close(client_fd);

  printf("Info: Connection Closed. Total Connections: %d\n",
         --tcp_server->concurrent_conn);
}
