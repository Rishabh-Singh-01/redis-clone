#include "./req.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

char CRLF[2] = {'\r', '\n'};
Request request;
Deserializer deserializer;
Serializer serializer;

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

void reset_request(Request *request) {
  request->req_arg_count = 10;
  request->req_arg_size = 10;
  request->req_arg_idx = 0;
  request->req_command_type = Req_Invalid;
  memset(request->req_args, '\0',
         sizeof(char) * request->req_arg_count * request->req_arg_size);
}

void initialize_deserializer(Deserializer *deserializer) {
  deserializer->bytes_read_count = 0;
  deserializer->read_buffer_offset_idx = 0;

  deserializer->crlf_visited_count = 0;
  deserializer->cur_arg_consumed_count = 0;
  deserializer->cur_arg_size = 0;
  deserializer->buffer_size = 1024;
  deserializer->buffer = (char *)malloc(sizeof(char) * 1024);
  memset(deserializer->buffer, '\0', sizeof(char) * 1024);
  deserializer->buffer_itr_idx = 0;
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

void execute_deserializer(Request *request, Deserializer *deserializer) {
  while (deserializer->buffer_itr_idx < deserializer->bytes_read_count) {

    bool is_idx_on_req_header = deserializer->crlf_visited_count == 0;
    bool is_idx_on_crlf =
        deserializer->buffer[deserializer->buffer_itr_idx] == CRLF[0] &&
        deserializer->buffer[deserializer->buffer_itr_idx + 1] == CRLF[1];
    bool is_idx_on_payload_header = deserializer->crlf_visited_count > 0 &&
                                    deserializer->crlf_visited_count % 2 != 0;
    bool is_idx_on_payload = deserializer->crlf_visited_count > 0 &&
                             deserializer->crlf_visited_count % 2 == 0;
    bool is_payload_consumed =
        deserializer->cur_arg_consumed_count == deserializer->cur_arg_size;

    if (!is_idx_on_crlf && (is_idx_on_req_header || is_idx_on_payload_header)) {
      deserializer->buffer_itr_idx++;
      continue;
    }

    if (is_idx_on_req_header && is_idx_on_crlf) {
      assert(deserializer->buffer[deserializer->buffer_itr_idx - 2] == '*');
      assert(deserializer->buffer[deserializer->buffer_itr_idx - 1] > '0');
      request->req_arg_count =
          deserializer->buffer[deserializer->buffer_itr_idx - 1] - '0';
      deserializer->buffer_itr_idx += 2;
      deserializer->crlf_visited_count++;
      continue;
    }

    if (is_idx_on_payload_header && is_idx_on_crlf) {
      assert(deserializer->buffer[deserializer->buffer_itr_idx - 2] == '$');
      assert(deserializer->buffer[deserializer->buffer_itr_idx - 1] > '0');
      deserializer->cur_arg_size =
          deserializer->buffer[deserializer->buffer_itr_idx - 1] - '0';
      deserializer->buffer_itr_idx += 2;
      deserializer->crlf_visited_count++;
      continue;
    }

    if (is_idx_on_payload && !is_payload_consumed) {
      int cur_arg_itr_idx = request->req_arg_idx * request->req_arg_size +
                            deserializer->cur_arg_consumed_count;
      request->req_args[cur_arg_itr_idx] =
          deserializer->buffer[deserializer->buffer_itr_idx];
      deserializer->cur_arg_consumed_count++;
      deserializer->buffer_itr_idx++;
      continue;
    }

    if (is_idx_on_payload && is_payload_consumed) {
      extract_command(request);

      request->req_arg_idx++;
      deserializer->buffer_itr_idx += 2;
      deserializer->crlf_visited_count++;
      deserializer->cur_arg_size = 0;
      deserializer->cur_arg_consumed_count = 0;
    }
  }
}

void execute_resp(int client_fd, TCP_Server *tcp_server) {
  // TODO: Only works assuming each "request" will be containing complete
  // commands

  initialize_request(&request);
  initialize_deserializer(&deserializer);
  while ((deserializer.bytes_read_count = read(client_fd, deserializer.buffer,
                                               deserializer.buffer_size)) > 0) {
    execute_deserializer(&request, &deserializer);

    init_serializer(&serializer);
    send_response(client_fd, &request, &serializer);

    initialize_request(&request);
    initialize_deserializer(&deserializer);
    // TODO: add logic to free mems as well
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
