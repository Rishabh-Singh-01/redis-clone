#include "./deserializer.h"
#include "command.h"
#include <stdio.h>
#include <string.h>

char CRLF[2] = {'\r', '\n'};

void init_command(Command *command) {
  command->command_type = Cmd_Invalid;
  command->argc = 0;
  command->args = NULL;
}

void reset_command(Command *command) {
  print_command(command);
  command->command_type = Cmd_Invalid;
  int idx = 0;
  while (idx < command->argc) {
    free(command->args[idx]);
    idx++;
  }
  free(command->args);
  command->argc = 0;
  command->args = NULL;
}

void initialize_request(Request *request) {
  request->req_arg_count = 10;
  request->req_arg_size = 10;
  request->req_arg_idx = 0;
  request->req_command_type = Cmd_Invalid;
  request->req_args = (char *)malloc(sizeof(char) * request->req_arg_count *
                                     request->req_arg_size);
  memset(request->req_args, '\0',
         sizeof(char) * request->req_arg_count * request->req_arg_size);
}

void cleanup_request(Request *request) { free(request->req_args); }

void debug_deserializer(Deserializer *deserializer) {
  printf("Des: deserializer->bytes_read_count - %d\n",
         deserializer->bytes_read_count);
  printf("Des: deserializer->read_buffer_offset_idx - %d\n",
         deserializer->read_buffer_offset_idx);
  printf("Des: deserializer->crlf_visited_count - %d\n",
         deserializer->crlf_visited_count);
  printf("Des: deserializer->cur_arg_consumed_count - %d\n",
         deserializer->cur_arg_consumed_count);
  printf("Des: deserializer->cur_arg_size - %d\n", deserializer->cur_arg_size);
  printf("Des: deserializer->buffer_size - %d\n", deserializer->buffer_size);
  printf("Des: deserializer->buffer - %s\n", deserializer->buffer);
  printf("Des: deserializer->buffer_itr_idx - %d\n",
         deserializer->buffer_itr_idx);
}

void cleanup_deserializer(Deserializer *deserializer) {
  deserializer->bytes_read_count = 0;
  deserializer->read_buffer_offset_idx = 0;
  deserializer->crlf_visited_count = 0;
  deserializer->cur_arg_consumed_count = 0;
  deserializer->cur_arg_size = 0;
  deserializer->buffer_size = 1024;
  deserializer->buffer_itr_idx = 0;
  free(deserializer->buffer);
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

void reset_deserializer(Deserializer *deserializer) {
  deserializer->bytes_read_count = 0;
  deserializer->read_buffer_offset_idx = 0;

  deserializer->crlf_visited_count = 0;
  deserializer->cur_arg_consumed_count = 0;
  deserializer->cur_arg_size = 0;
  deserializer->buffer_size = 1024;
  deserializer->buffer_itr_idx = 0;
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
      assert(deserializer->buffer[deserializer->buffer_itr_idx - 2] == '$' ||
             deserializer->buffer[deserializer->buffer_itr_idx - 3] == '$');
      deserializer->cur_arg_size =
          // NOTE: so bad that for timestamp i am literally hardcoding 10 hahaha
          deserializer->buffer[deserializer->buffer_itr_idx - 2] == '$'
              ? deserializer->buffer[deserializer->buffer_itr_idx - 1] - '0'
              : 10;
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

void init_req_parser(Req_Parser *req_parser) {
  req_parser->buffer_size = 100;
  req_parser->buffer = malloc(sizeof(char) * req_parser->buffer_size);
}

void reset_req_parser(Req_Parser *req_parser) {
  req_parser->buffer_size = 100;
  memset(req_parser->buffer, '\0', req_parser->buffer_size);
}

// NOTE: very simple (wrong) parser, make it better for tcp
// Currently it assumes we will be getting the full cmd

void exec_req_parser(Req_Parser *req_parser, Command *command) {
  char *temp = malloc(sizeof(char) * req_parser->buffer_size);
  memcpy(temp, req_parser->buffer, req_parser->buffer_size);
  const char delimiter[] = "\r\n";
  char *token = strtok(temp, delimiter);
  int token_number = 0;
  while (token != NULL) {
    printf("Token: %s\n", token);
    if (token_number == 0) {
      // Token (eg: *2), to "remove *" we are adding 1
      command->argc = atoi(token + 1) - 1;
      command->args = (char **)malloc(sizeof(char **) * command->argc);
    } else if (token_number == 2) {
      if (strcasecmp(token, "PING") == 0) {
        command->command_type = Cmd_Ping;
      } else if (strcasecmp(token, "ECHO") == 0) {
        command->command_type = Cmd_Echo;
      } else if (strcasecmp(token, "GET") == 0) {
        command->command_type = Cmd_Get;
      } else if (strcasecmp(token, "SET") == 0) {
        command->command_type = Cmd_Set;
      } else if (strcasecmp(token, "DEL") == 0) {
        command->command_type = Cmd_Del;
      } else if (strcasecmp(token, "Expire") == 0) {
        command->command_type = Cmd_Expire;
      } else if (strcasecmp(token, "TTL") == 0) {
        command->command_type = Cmd_TTL;
      } else if (strcasecmp(token, "ECHO") == 0) {
        command->command_type = Cmd_Echo;
      } else {
        command->command_type = Cmd_Invalid;
      }
    } else if (token_number % 2 == 0) {
      int idx = (token_number / 2) - 2;
      command->args[idx] = (char *)malloc(sizeof(char) * strlen(token));
      strcpy(command->args[idx], token);
    } else {
    }

    token_number++;
    token = strtok(NULL, delimiter);
  }

  free(temp);
}

void print_command(Command *command) {
  printf("Command Type: %d\n", command->command_type);
  printf("Command Args Count: %d\n", command->argc);

  int idx = 0;
  while (idx < command->argc) {
    printf("Command Arg (%d): %s\n", idx, command->args[idx]);
    idx++;
  }
}
