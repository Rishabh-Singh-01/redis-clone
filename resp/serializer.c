#include "./serializer.h"
#include <stdio.h>
#include <string.h>

void normalize_command(Command *command) {
  if (command->command_type == Cmd_Set) {
    // do something
  }
}

void cleanup_serializer(Serializer *serializer) {
  serializer->buffer_itr_idx = 0;
  serializer->buffer_size = 100;
  free(serializer->buffer);
}

void init_serializer(Serializer *serializer) {
  serializer->buffer_itr_idx = 0;
  serializer->buffer_size = 100;
  serializer->buffer = (char *)malloc(sizeof(char) * serializer->buffer_size);
  memset(serializer->buffer, '\0', sizeof(char) * 100);
}

void send_response(int client_fd, Request *request, Response *response,
                   Serializer *serializer) {
  if (request->req_command_type == Cmd_Ping) {
    memcpy(serializer->buffer, "+PONG\r\n", strlen("+PONG\r\n"));

  } else if (request->req_command_type == Cmd_Set) {
    // TODO: this kind of check should happen before the dispatch
    if (request->req_arg_count != 3 && request->req_arg_count != 5) {
      char *err_msg = "-ERR wrong number of arguments for 'set' command\r\n";
      memcpy(serializer->buffer, err_msg, strlen(err_msg));

    } else if (request->req_arg_count == 5 &&
               (strcasecmp(request->req_args + (3 * request->req_arg_size),
                           "EXAT") != 0)) {
      char *err_msg = "-ERR sytax error for 'set' command\r\n";
      memcpy(serializer->buffer, err_msg, strlen(err_msg));

    } else {
      char *res_msg = "+OK\r\n";
      memcpy(serializer->buffer, res_msg, strlen(res_msg));
    }

  } else if (request->req_command_type == Cmd_Del) {
    serializer->buffer[serializer->buffer_itr_idx++] = ':';
    char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
    strcpy(buf_cp_idx, response->result);
    serializer->buffer_itr_idx += strlen(response->result);
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];

  } else if (request->req_command_type == Cmd_Expire) {
    serializer->buffer[serializer->buffer_itr_idx++] = ':';
    char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
    strcpy(buf_cp_idx, response->result);
    serializer->buffer_itr_idx += strlen(response->result);
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];

  } else if (request->req_command_type == Cmd_TTL) {
    serializer->buffer[serializer->buffer_itr_idx++] = ':';
    char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
    strcpy(buf_cp_idx, response->result);
    serializer->buffer_itr_idx += strlen(response->result);
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];

  } else if (request->req_command_type == Cmd_Echo) {
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

  } else if (request->req_command_type == Cmd_Get) {
    if (response->result == NULL) {
      char *res_msg = "$-1\r\n";
      memcpy(serializer->buffer, res_msg, strlen(res_msg));

    } else {

      char *res_arg_idx = response->result;
      int res_arg_size = strlen(res_arg_idx);
      serializer->buffer[serializer->buffer_itr_idx++] = '$';
      serializer->buffer[serializer->buffer_itr_idx++] = res_arg_size + '0';
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
      char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
      memcpy(buf_cp_idx, res_arg_idx, res_arg_size);
      serializer->buffer_itr_idx += res_arg_size;
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
    }
  } else {
  }

  int byte_sent =
      send(client_fd, serializer->buffer, strlen(serializer->buffer), 0);
  if (byte_sent < 0) {
    perror("Error: Failed to write to client");
  }
}

void generate_response(Command *command, Response *response,
                       Serializer *serializer) {
  if (command->command_type == Cmd_Ping) {
    memcpy(serializer->buffer, "+PONG\r\n", strlen("+PONG\r\n"));

  } else if (command->command_type == Cmd_Set) {
    // TODO: this kind of check should happen before the dispatch
    if (command->argc != 2 && command->argc != 4) {
      char *err_msg = "-ERR wrong number of arguments for 'set' command\r\n";
      memcpy(serializer->buffer, err_msg, strlen(err_msg));

    } else if (command->argc == 4 &&
               (strcasecmp(command->args[3], "EXAT") != 0)) {
      char *err_msg = "-ERR sytax error for 'set' command\r\n";
      memcpy(serializer->buffer, err_msg, strlen(err_msg));

    } else {
      char *res_msg = "+OK\r\n";
      memcpy(serializer->buffer, res_msg, strlen(res_msg));
    }

  } else if (command->command_type == Cmd_Del) {
    serializer->buffer[serializer->buffer_itr_idx++] = ':';
    char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
    strcpy(buf_cp_idx, response->result);
    serializer->buffer_itr_idx += strlen(response->result);
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];

  } else if (command->command_type == Cmd_Expire) {
    if (command->argc != 2) {
      char *err_msg = "-ERR wrong number of arguments for 'expire' command\r\n";
      memcpy(serializer->buffer, err_msg, strlen(err_msg));
    } else {
      serializer->buffer[serializer->buffer_itr_idx++] = ':';
      char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
      strcpy(buf_cp_idx, response->result);
      serializer->buffer_itr_idx += strlen(response->result);
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
    }

  } else if (command->command_type == Cmd_TTL) {
    if (command->argc != 1) {
      char *err_msg = "-ERR wrong number of arguments for 'ttl' command\r\n";
      memcpy(serializer->buffer, err_msg, strlen(err_msg));
    } else {
      serializer->buffer[serializer->buffer_itr_idx++] = ':';
      char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
      strcpy(buf_cp_idx, response->result);
      serializer->buffer_itr_idx += strlen(response->result);
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
    }

  } else if (command->command_type == Cmd_Echo) {
    char *first_arg_idx = command->args[0];
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

  } else if (command->command_type == Cmd_Get) {
    if (response->result == NULL) {
      char *res_msg = "$-1\r\n";
      memcpy(serializer->buffer, res_msg, strlen(res_msg));

    } else {

      char *res_arg_idx = response->result;
      int res_arg_size = strlen(res_arg_idx);
      serializer->buffer[serializer->buffer_itr_idx++] = '$';
      serializer->buffer[serializer->buffer_itr_idx++] = res_arg_size + '0';
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
      char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
      memcpy(buf_cp_idx, res_arg_idx, res_arg_size);
      serializer->buffer_itr_idx += res_arg_size;
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
    }
  } else {
    char *err_msg = "-ERR unknown command\r\n";
    memcpy(serializer->buffer, err_msg, strlen(err_msg));
  }
}

void send_response_only(int client_fd, Serializer *serializer) {
  int byte_sent =
      send(client_fd, serializer->buffer, strlen(serializer->buffer), 0);
  if (byte_sent < 0) {
    perror("Error: Failed to write to client");
  }
}

void send_response_new(int client_fd, Command *command, Response *response,
                       Serializer *serializer) {
  if (command->command_type == Cmd_Ping) {
    memcpy(serializer->buffer, "+PONG\r\n", strlen("+PONG\r\n"));

  } else if (command->command_type == Cmd_Set) {
    // TODO: this kind of check should happen before the dispatch
    if (command->argc != 2 && command->argc != 4) {
      char *err_msg = "-ERR wrong number of arguments for 'set' command\r\n";
      memcpy(serializer->buffer, err_msg, strlen(err_msg));

    } else if (command->argc == 4 &&
               (strcasecmp(command->args[3], "EXAT") != 0)) {
      char *err_msg = "-ERR sytax error for 'set' command\r\n";
      memcpy(serializer->buffer, err_msg, strlen(err_msg));

    } else {
      char *res_msg = "+OK\r\n";
      memcpy(serializer->buffer, res_msg, strlen(res_msg));
    }

  } else if (command->command_type == Cmd_Del) {
    serializer->buffer[serializer->buffer_itr_idx++] = ':';
    char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
    strcpy(buf_cp_idx, response->result);
    serializer->buffer_itr_idx += strlen(response->result);
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
    serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];

  } else if (command->command_type == Cmd_Expire) {
    if (command->argc != 2) {
      char *err_msg = "-ERR wrong number of arguments for 'expire' command\r\n";
      memcpy(serializer->buffer, err_msg, strlen(err_msg));
    } else {
      serializer->buffer[serializer->buffer_itr_idx++] = ':';
      char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
      strcpy(buf_cp_idx, response->result);
      serializer->buffer_itr_idx += strlen(response->result);
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
    }

  } else if (command->command_type == Cmd_TTL) {
    if (command->argc != 1) {
      char *err_msg = "-ERR wrong number of arguments for 'ttl' command\r\n";
      memcpy(serializer->buffer, err_msg, strlen(err_msg));
    } else {
      serializer->buffer[serializer->buffer_itr_idx++] = ':';
      char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
      strcpy(buf_cp_idx, response->result);
      serializer->buffer_itr_idx += strlen(response->result);
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
    }

  } else if (command->command_type == Cmd_Echo) {
    char *first_arg_idx = command->args[0];
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

  } else if (command->command_type == Cmd_Get) {
    if (response->result == NULL) {
      char *res_msg = "$-1\r\n";
      memcpy(serializer->buffer, res_msg, strlen(res_msg));

    } else {

      char *res_arg_idx = response->result;
      int res_arg_size = strlen(res_arg_idx);
      serializer->buffer[serializer->buffer_itr_idx++] = '$';
      serializer->buffer[serializer->buffer_itr_idx++] = res_arg_size + '0';
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
      char *buf_cp_idx = serializer->buffer + serializer->buffer_itr_idx;
      memcpy(buf_cp_idx, res_arg_idx, res_arg_size);
      serializer->buffer_itr_idx += res_arg_size;
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[0];
      serializer->buffer[serializer->buffer_itr_idx++] = CRLF[1];
    }
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
