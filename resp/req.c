#include "./req.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

char CRLF[2] = {'\r', '\n'};

void initialize_request(Request *request) {
  request->req_arg_count = 10;
  request->req_arg_size = 10;
  request->req_arg_idx = 0;
  request->req_command_type = Req_Invalid;
  request->req_args = (char *)malloc(sizeof(char) * request->req_arg_count *
                                     request->req_arg_size);
  memset(request->req_args, '\0',
         sizeof(char) * request->req_arg_count * request->req_arg_size);
}

void initialize_deserializer(Deserializer *state) {
  state->bytes_read_count = 0;
  state->read_buffer_offset_idx = 0;

  state->crlf_visited_count = 0;
  state->cur_arg_consumed_count = 0;
  state->cur_arg_size = 0;
  state->buffer_size = 1024;
  state->buffer = (char *)malloc(sizeof(char) * 1024);
  memset(state->buffer, '\0', sizeof(char) * 1024);
  state->buffer_itr_idx = 0;
}

void extract_command(Request *request) {
  if (strcasecmp(request->req_args, "PING") == 0) {
    request->req_command_type = Req_Ping;
  } else if (strcasecmp(request->req_args, "ECHO") == 0) {
    request->req_command_type = Req_Echo;
  } else {
    request->req_command_type = Req_Invalid;
  }
}

void print_hex(const char *s) {
  while (*s) {
    // %02x ensures at least 2 digits with leading zeros
    printf("%02x ", (unsigned char)*s++);
  }
  printf("\n");
}

void send_response(int client_fd, Request *request) {
  char *res;
  int res_size = 0;
  char *ptr = malloc(sizeof(char) * 100);
  memset(ptr, '\0', sizeof(char) * 100);

  if (request->req_command_type == Req_Ping) {
    res = "+PONG\r\n";
    res_size = strlen(res);
  } else if (request->req_command_type == Req_Echo) {
    printf("echo response: %s\n", request->req_args + request->req_arg_size);
    char *res_temp = request->req_args + request->req_arg_size;
    int res_temp_size = strlen(res_temp);
    ptr[0] = '$';
    ptr[1] = res_temp_size + '0';
    ptr[2] = '\r';
    ptr[3] = '\n';
    strncpy(ptr + 4, res_temp, res_temp_size);
    int new_res_size = strlen(ptr);
    ptr[new_res_size] = '\r';
    ptr[new_res_size + 1] = '\n';
    print_hex(ptr);
    res = ptr;
    res_size = strlen(ptr);
  } else {
    printf("Random Response\n");
    res = "+PONG\r\n";
    res_size = strlen(res);
  }
  int byte_sent = send(client_fd, res, res_size, 0);
  if (byte_sent < 0) {
    perror("Error: Failed to write to client");
  }
}

void deserialize_request(int client_fd, TCP_Server *tcp_server) {
  // TODO: Update this to ensure that the single bytes send are also working
  Request request;
  initialize_request(&request);

  Deserializer deserializer;
  initialize_deserializer(&deserializer);

  while ((deserializer.bytes_read_count = read(client_fd, deserializer.buffer,
                                               deserializer.buffer_size)) > 0) {

    while (deserializer.buffer_itr_idx < deserializer.bytes_read_count) {
      if (deserializer.crlf_visited_count == 0 &&
          deserializer.buffer[deserializer.buffer_itr_idx] == CRLF[0] &&
          deserializer.buffer[deserializer.buffer_itr_idx + 1] == CRLF[1]) {

        assert(deserializer.buffer[deserializer.buffer_itr_idx - 2] == '*' &&
               "Error: Request should start with Arr Command");
        assert(deserializer.buffer[deserializer.buffer_itr_idx - 1] > '0' &&
               "Error: Should have more than 0 args");

        deserializer.crlf_visited_count++;
        request.req_arg_count =
            deserializer.buffer[deserializer.buffer_itr_idx - 1] - '0';
        deserializer.buffer_itr_idx += 2;
        continue;
      }

      if (deserializer.crlf_visited_count > 0 &&
          deserializer.crlf_visited_count % 2 != 0 &&
          (deserializer.buffer[deserializer.buffer_itr_idx] == CRLF[0] &&
           deserializer.buffer[deserializer.buffer_itr_idx + 1] == CRLF[1])) {

        assert(deserializer.buffer[deserializer.buffer_itr_idx - 2] == '$' &&
               "Error: Request should start with Arr Command");
        assert(deserializer.buffer[deserializer.buffer_itr_idx - 1] > '0' &&
               "Error: Should have more than 0 args");

        deserializer.crlf_visited_count++;
        deserializer.cur_arg_size =
            deserializer.buffer[deserializer.buffer_itr_idx - 1] - '0';
        deserializer.buffer_itr_idx += 2;
        continue;
      }

      if (deserializer.crlf_visited_count > 0 &&
          deserializer.crlf_visited_count % 2 == 0) {
        request.req_args[request.req_arg_idx * request.req_arg_size +
                         deserializer.cur_arg_consumed_count] =
            deserializer.buffer[deserializer.buffer_itr_idx];
        deserializer.cur_arg_consumed_count++;

        if (deserializer.cur_arg_consumed_count == deserializer.cur_arg_size) {
          if (deserializer.crlf_visited_count == 2) {
            extract_command(&request);
          }

          request.req_arg_idx++;
          deserializer.cur_arg_consumed_count = 0;
          deserializer.cur_arg_size = 0;
          deserializer.buffer_itr_idx += 3;
          deserializer.crlf_visited_count++;
          continue;
        }
      }
      deserializer.buffer_itr_idx++;
    }

    send_response(client_fd, &request);

    initialize_deserializer(&deserializer);
    initialize_request(&request);
  }

  if (deserializer.bytes_read_count == 0) {
    printf("Info: Connection closed\n");
  } else {
    perror("Error: Issue with connectio during close\n");
  }
  close(client_fd);

  printf("Info: Connection Closed. Total Connections: %d\n",
         --tcp_server->concurrent_conn);
}
