#include "./req.h"
#include <arpa/inet.h>
#include <stdio.h>

char CRLF[2] = {'\r', '\n'};

void deserialize_request(int client_fd, TCP_Server *tcp_server) {
  printf("Inside the deserialize_request\n");
  Request request = {};
  request.args = 0;
  request.requestType = (RequestType *)malloc(sizeof(RequestType) * 10);

  int read_count;
  int buffer_offset = 0;
  int crlf_count = 0;
  int req_byte_consumed = 0;
  int req_byte_size = 0;
  char *req_bytes = malloc(sizeof(char) * 10);

  while ((read_count = read(client_fd, tcp_server->buffer + buffer_offset,
                            tcp_server->buffer_size - buffer_offset)) > 0) {

    if (crlf_count <= (2 * request.args)) {
      printf("Inside the read loop\n");
      int buffer_itr = buffer_offset;
      buffer_offset += read_count;
      printf("Buffer Itr: %d; Buffer Offset: %d\n", buffer_itr, buffer_offset);
      printf("Flushed Buffer: %s", tcp_server->buffer);

      while (buffer_itr < buffer_offset - 1) {
        printf("Buffer Itr: %d; tcp_server->buffer[buffer_itr]: %d\n",
               buffer_itr, tcp_server->buffer[buffer_itr]);
        printf("Bool - CRLF[0]: %d\n",
               tcp_server->buffer[buffer_itr] == CRLF[0]);
        printf("Bool - CRLF[1]: %d\n",
               tcp_server->buffer[buffer_itr + 1] == CRLF[1]);
        if (crlf_count == 0 && tcp_server->buffer[buffer_itr] == CRLF[0] &&
            tcp_server->buffer[buffer_itr + 1] == CRLF[1]) {
          printf("Inside the crlf_count = 0\n");
          crlf_count++;

          assert(tcp_server->buffer[buffer_itr - 2] == '*' &&
                 "Error: Request should start with Arr Command");
          assert(tcp_server->buffer[buffer_itr - 1] > '0' &&
                 "Error: Should have more than 0 args");

          request.args = tcp_server->buffer[buffer_itr - 1] - '0';
        } else {

          if ((crlf_count > 0) && (crlf_count <= (2 * request.args))) {
            if (crlf_count % 2 != 0 &&
                (tcp_server->buffer[buffer_itr] == CRLF[0] &&
                 tcp_server->buffer[buffer_itr + 1] == CRLF[1])) {

              printf("Inside this\n");
              assert(tcp_server->buffer[buffer_itr - 2] == '$' &&
                     "Error: Request should start with Arr Command");
              assert(tcp_server->buffer[buffer_itr - 1] > '0' &&
                     "Error: Should have more than 0 args");

              crlf_count++;
              req_byte_size = tcp_server->buffer[buffer_itr - 1] - '0';
              printf("req_byte_size: %d\n", req_byte_size);
            } else if (crlf_count % 2 == 0 &&
                       tcp_server->buffer[buffer_itr] != CRLF[1]) {

              req_bytes[req_byte_consumed] = tcp_server->buffer[buffer_itr];
              req_byte_consumed++;

              printf("Inside that ,req_byte_consumed: %d\n", req_byte_consumed);
              if (req_byte_consumed == req_byte_size) {
                req_bytes[req_byte_consumed + 1] = '\0';
                printf("> %s\n", req_bytes);
                req_byte_consumed = 0;
                req_byte_size = 0;
                buffer_itr += 2;
                crlf_count++;
                printf("crlf_count: %d\n", crlf_count);
                printf("request args: %d\n", request.args);
              }
            }
          }
        }
        buffer_itr++;
      }
      printf("Something here\n");
    } else {
      printf("Not sure\n");
      int byte_sent = send(client_fd, "+PONG\r\n", 7, 0);
      if (byte_sent < 0) {
        perror("Error: Failed to write to client");
      }
      read_count = 0;
      buffer_offset = 0;
      crlf_count = 0;
      req_byte_consumed = 0;
      req_byte_size = 0;
    }
  }
}
