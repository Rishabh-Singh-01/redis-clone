#include "./executor.h"
#include "./../engine/hashmap.h"
#include <stdio.h>

void init_command(Command *command) {
  command->command_type = Cmd_Invalid;
  command->argc = 0;
  command->args = NULL;
}

void reset_command(Command *command) {
  // print_command(command);
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

bool validate_command(Command *command, Serializer *serializer) {
  char *err_msg;
  bool is_valid_command = false;
  if (command->command_type == Cmd_Set) {

    printf("Command Type: %d\n", command->command_type);
    printf("Command args: %d\n", command->argc);
    printf("Command excomp: %d\n", command->command_type == Cmd_Set &&
                                       command->argc == 4 &&
                                       (strcasecmp(command->args[2], "ex")));
  }

  if (command->command_type == Cmd_Get && command->argc != 1) {
    err_msg = "-ERR wrong number of arguments for 'get' command\r\n";
  } else if (command->command_type == Cmd_Set &&
             !(command->argc == 2 || command->argc == 4)) {
    err_msg = "-ERR wrong number of arguments for 'set' command\r\n";
  } else if (command->command_type == Cmd_Set && command->argc == 4 &&
             (strcasecmp(command->args[2], "ex") != 0)) {
    err_msg = "-ERR invalid syntax for 'set' command\r\n";
  } else if (command->command_type == Cmd_Del && command->argc != 1) {
    err_msg = "-ERR wrong number of arguments for 'del' command\r\n";
  } else if (command->command_type == Cmd_Expire && command->argc != 2) {
    err_msg = "-ERR wrong number of arguments for 'expire' command\r\n";
  } else if (command->command_type == Cmd_TTL && command->argc != 1) {
    err_msg = "-ERR wrong number of arguments for 'ttl' command\r\n";
  } else if (command->command_type == Cmd_Echo && command->argc != 1) {
    err_msg = "-ERR wrong number of arguments for 'echo' command\r\n";
  } else if (command->command_type == Cmd_Ping && command->argc != 0) {
    err_msg = "-ERR wrong number of arguments for 'echo' command\r\n";
  } else if (command->command_type == Cmd_Invalid) {
    err_msg = "-ERR unknown command\r\n";
  } else {
    is_valid_command = true;
  }

  if (!is_valid_command) {
    memcpy(serializer->buffer, err_msg, strlen(err_msg));
  }
  return is_valid_command;
}

void execute_norm_command(Storage_hashmap *st_map, Response *response,
                          Command *command) {
  switch (command->command_type) {
  case Cmd_Get: {
    Storage_hashmap_internal_ll *res =
        get_kv_in_hashmap(st_map, command->args[0]);
    if (res == NULL) {
      response->type = Res_Null;
      break;
    }
    if (res->expires_at != 0 && time(NULL) > res->expires_at) {
      // NOTE: htis is redundant, we are iterating over hmap twice
      delete_kv_in_hashmap(st_map, res->key);
      response->type = Res_Null;
      break;
    }

    response->type = Res_Bulk_String;
    response->argc = 1;
    response->args = (char **)malloc(sizeof(char *) * response->argc);
    response->args[0] = (char *)malloc(sizeof(char) * strlen(res->value));
    strcpy(response->args[0], res->value);
    break;
  }
  case Cmd_Set: {
    time_t expiration_at = atoi(command->args[3]);
    printf("Command C: %d\n", command->argc);
    printf("Command: %s\n", command->args[0]);
    printf("Command: %s\n", command->args[1]);

    set_kv_in_hashmap(st_map, command->args[0], command->args[1],
                      expiration_at);
    response->type = Res_Simple_String;
    response->argc = 1;
    response->args = (char **)malloc(sizeof(char *) * response->argc);
    response->args[0] = (char *)malloc(sizeof(char) * 2);
    strcpy(response->args[0], "OK");
    break;
  }
  case Cmd_Del: {
    int deleted_count = 0;
    for (int i = 0; i < command->argc; i++) {
      char *key = command->args[i];
      bool is_deleted = delete_kv_in_hashmap(st_map, key);
      deleted_count += is_deleted ? 1 : 0;
    }
    response->type = Res_Integer;
    response->argc = 1;
    response->args = (char **)malloc(sizeof(char *) * response->argc);
    // 2 since i am not going to be deleting more than 2 elems
    response->args[0] = (char *)malloc(sizeof(char) * 2);
    snprintf(response->args[0], sizeof(response->args[0]), "%d", deleted_count);
    break;
  }
  case ICmd_Exat: {
  }
  case Cmd_Expire: {
    Storage_hashmap_internal_ll *res =
        get_kv_in_hashmap(st_map, command->args[0]);
    if (res == NULL) {
      response->type = Res_Integer;
      response->argc = 1;
      response->args = (char **)malloc(sizeof(char *) * response->argc);
      response->args[0] = (char *)malloc(sizeof(char) * 1);
      snprintf(response->args[0], sizeof(response->args[0]), "%d", 0);
      break;
    }
    if (res->expires_at != 0 && time(NULL) > res->expires_at) {
      // NOTE: htis is redundant, we are iterating over hmap twice
      delete_kv_in_hashmap(st_map, res->key);
      response->type = Res_Integer;
      response->argc = 1;
      response->args = (char **)malloc(sizeof(char *) * response->argc);
      response->args[0] = (char *)malloc(sizeof(char) * 1);
      snprintf(response->args[0], sizeof(response->args[0]), "%d", 0);
      break;
    }

    char *temp;
    time_t expires_at = (time_t)strtoll(command->args[1], &temp, 10);
    res->expires_at = expires_at;
    response->type = Res_Integer;
    response->argc = 1;
    response->args = (char **)malloc(sizeof(char *) * response->argc);
    response->args[0] = (char *)malloc(sizeof(char) * 1);
    snprintf(response->args[0], sizeof(response->args[0]), "%d", 1);
    break;
  }
  case Cmd_TTL: {
    Storage_hashmap_internal_ll *res =
        get_kv_in_hashmap(st_map, command->args[0]);
    if (res == NULL) {
      response->type = Res_Integer;
      response->argc = 1;
      response->args = (char **)malloc(sizeof(char *) * response->argc);
      response->args[0] = (char *)malloc(sizeof(char) * 1);
      snprintf(response->args[0], sizeof(response->args[0]), "%d", -2);
      break;
    }
    if (res->expires_at == 0) {
      response->type = Res_Integer;
      response->argc = 1;
      response->args = (char **)malloc(sizeof(char *) * response->argc);
      response->args[0] = (char *)malloc(sizeof(char) * 1);
      snprintf(response->args[0], sizeof(response->args[0]), "%d", -1);
      break;
    }
    if (time(NULL) > res->expires_at) {
      // NOTE: htis is redundant, we are iterating over hmap twice
      delete_kv_in_hashmap(st_map, res->key);
      response->type = Res_Integer;
      response->argc = 1;
      response->args = (char **)malloc(sizeof(char *) * response->argc);
      response->args[0] = (char *)malloc(sizeof(char) * 1);
      snprintf(response->args[0], sizeof(response->args[0]), "%d", -2);
      break;
    }

    time_t remaining_time = res->expires_at - time(NULL);
    response->type = Res_Integer;
    response->argc = 1;
    response->args = (char **)malloc(sizeof(char *) * response->argc);
    response->args[0] = (char *)malloc(sizeof(char) * 2);
    snprintf(response->args[0], sizeof(response->args[0]), "%d",
             (int)remaining_time);
    break;
  }
  case Cmd_Ping: {
    response->type = Res_Simple_String;
    response->argc = 1;
    response->args = (char **)malloc(sizeof(char *) * response->argc);
    response->args[0] = (char *)malloc(sizeof(char) * 4);
    strcpy(response->args[0], "Pong");
    break;
  }
  case Cmd_Echo: {
    response->type = Res_Bulk_String;
    response->argc = 1;
    response->args = (char **)malloc(sizeof(char *) * response->argc);
    response->args[0] = (char *)malloc(sizeof(char) * strlen(command->args[0]));
    strcpy(response->args[0], command->args[0]);
    break;
  }
  default: {
    response->type = Res_Simple_String;
    response->argc = 1;
    response->args = (char **)malloc(sizeof(char *) * response->argc);
    response->args[0] = (char *)malloc(sizeof(char) * 4);
    strcpy(response->args[0], "Pong");
  }
  }
}

// void dispatch_command(Storage_hashmap *st_map, Request *request,
//                       Response *response) {
//   switch (request->req_command_type) {
//   case Cmd_Get: {
//     Storage_hashmap_internal_ll *res = get_kv_in_hashmap(
//         st_map, request->req_args + (1 * request->req_arg_size));
//     if (res == NULL) {
//       break;
//     }
//     if (res != NULL && res->expires_at != 0 && time(NULL) > res->expires_at)
//     {
//       // NOTE: htis is redundant, we are iterating over hmap twice
//       delete_kv_in_hashmap(st_map, res->key);
//       break;
//     }
//
//     strcpy(response->result, res->value);
//     break;
//   }
//   case Cmd_Set: {
//     // time_t expiration_at =
//     //     request->req_arg_count == 5
//     //         ? time(NULL) + atoi(request->req_args + (4 *
//     //         request->req_arg_size)) : 0;
//
//     char *temp;
//     time_t expiration_at = (time_t)strtoll(
//         request->req_args + (4 * request->req_arg_size), &temp, 10);
//     set_kv_in_hashmap(st_map, request->req_args + (1 *
//     request->req_arg_size),
//                       request->req_args + (2 * request->req_arg_size),
//                       expiration_at);
//     break;
//   }
//   case Cmd_Del: {
//     int deleted_count = 0;
//     for (int i = 1; i < request->req_arg_count; i++) {
//       char *key = request->req_args + (i * request->req_arg_size);
//       bool is_deleted = delete_kv_in_hashmap(st_map, key);
//       deleted_count += is_deleted ? 1 : 0;
//     }
//     snprintf(response->result, sizeof(response->result), "%d",
//     deleted_count); break;
//   }
//   case Cmd_Expire: {
//     Storage_hashmap_internal_ll *res = get_kv_in_hashmap(
//         st_map, request->req_args + (1 * request->req_arg_size));
//     if (res == NULL) {
//       snprintf(response->result, sizeof(response->result), "%d", 0);
//       break;
//     }
//     if (res != NULL && res->expires_at != 0 && time(NULL) > res->expires_at)
//     {
//       // NOTE: htis is redundant, we are iterating over hmap twice
//       delete_kv_in_hashmap(st_map, res->key);
//       snprintf(response->result, sizeof(response->result), "%d", 0);
//       break;
//     }
//
//     time_t expiration_at =
//         time(NULL) + atoi(request->req_args + (2 * request->req_arg_size));
//     res->expires_at = expiration_at;
//     snprintf(response->result, sizeof(response->result), "%d", 1);
//     break;
//   }
//   case Cmd_TTL: {
//     Storage_hashmap_internal_ll *res = get_kv_in_hashmap(
//         st_map, request->req_args + (1 * request->req_arg_size));
//     if (res == NULL) {
//       snprintf(response->result, sizeof(response->result), "%d", -2);
//       break;
//     }
//     if (res != NULL && res->expires_at == 0) {
//       snprintf(response->result, sizeof(response->result), "%d", -1);
//       break;
//     }
//     if (res != NULL && time(NULL) > res->expires_at) {
//       // NOTE: htis is redundant, we are iterating over hmap twice
//       delete_kv_in_hashmap(st_map, res->key);
//       snprintf(response->result, sizeof(response->result), "%d", -2);
//       break;
//     }
//
//     time_t remaining_time = res->expires_at - time(NULL);
//     snprintf(response->result, sizeof(response->result), "%d",
//              (int)remaining_time);
//     break;
//   }
//   default: {
//     // do nothing
//   }
//   }
// }

void normalize_command(Command *command) {
  if (command->command_type == Cmd_Set) {
    if (command->argc == 4) {
      time_t expires_at = time(NULL) + atoi(command->args[3]);
      char *sym_exp_at = "exat";
      free(command->args[2]);
      command->args[2] = (char *)malloc(sizeof(char) * strlen(sym_exp_at));
      strcpy(command->args[2], sym_exp_at);
      free(command->args[3]);
      command->args[3] =
          (char *)malloc(sizeof(char) * 10); // 10 is the "len" of timestamp
      sprintf(command->args[3], "%lld", (long long)expires_at);
    } else {
      char **p_args = (char **)realloc(command->args, sizeof(char **) * 4);
      char *sym_exp_at = "exat";
      command->args = p_args;
      command->args[2] = (char *)malloc(sizeof(char) * 4);
      strcpy(command->args[2], sym_exp_at);
      command->args[3] = (char *)malloc(sizeof(char) * 1);
      sprintf(command->args[3], "%d", 0);
    }
  } else if (command->command_type == Cmd_Expire) {
    time_t expires_at = time(NULL) + atoi(command->args[1]);
    free(command->args[1]);
    command->args[1] = (char *)malloc(sizeof(char) * 10);
    sprintf(command->args[1], "%lld", (long long)expires_at);
  } else {
  }
}
