#include "./deserializer.h"

char CRLF[2] = {'\r', '\n'};

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
      } else if (strcasecmp(token, "Exat") == 0) {
        command->command_type = ICmd_Exat;
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

// TODO: make this better, mehtod is not good
char *cmd_to_req(Command *command) {
  char *buf = (char *)malloc(sizeof(char) * 100);
  memset(buf, '\0', 100);

  if (command->command_type == Cmd_Set) {
    sprintf(buf,
            "*%d\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%"
            "d\r\n%s\r\n",
            5, 3, "set", (int)strlen(command->args[0]), command->args[0],
            (int)strlen(command->args[1]), command->args[1],
            (int)strlen(command->args[2]), command->args[2],
            (int)strlen(command->args[3]), command->args[3]);
  } else if (command->command_type == Cmd_Expire) {
    sprintf(buf, "*%d\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n", 3, 4, "exat",
            (int)strlen(command->args[0]), command->args[0],
            (int)strlen(command->args[1]), command->args[1]);
  } else if (command->command_type == Cmd_Del) {
    sprintf(buf, "*%d\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n", 2, 3, "del",
            (int)strlen(command->args[0]), command->args[0]);
  } else {
  }

  return buf;
}
