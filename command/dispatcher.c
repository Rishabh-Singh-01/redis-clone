#include "./dispatcher.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void execute_norm_command(Storage_hashmap *st_map, Response *response,
                          Command *command) {
  switch (command->command_type) {
  case Cmd_Get: {
    Storage_hashmap_internal_ll *res =
        get_kv_in_hashmap(st_map, command->args[0]);
    if (res == NULL) {
      break;
    }
    if (res != NULL && res->expires_at != 0 && time(NULL) > res->expires_at) {
      // NOTE: htis is redundant, we are iterating over hmap twice
      delete_kv_in_hashmap(st_map, res->key);
      break;
    }

    strcpy(response->result, res->value);
    break;
  }
  case Cmd_Set: {
    time_t expiration_at =
        command->argc == 4 ? time(NULL) + atoi(command->args[3]) : 0;

    // char *temp;
    // time_t expiration_at = (time_t)strtoll(
    //     request->req_args + (4 * request->req_arg_size), &temp, 10);
    set_kv_in_hashmap(st_map, command->args[0], command->args[1],
                      expiration_at);
    break;
  }
  case Cmd_Del: {
    int deleted_count = 0;
    for (int i = 0; i < command->argc; i++) {
      char *key = command->args[i];
      bool is_deleted = delete_kv_in_hashmap(st_map, key);
      deleted_count += is_deleted ? 1 : 0;
    }
    snprintf(response->result, sizeof(response->result), "%d", deleted_count);
    break;
  }
  case Cmd_Expire: {
    Storage_hashmap_internal_ll *res =
        get_kv_in_hashmap(st_map, command->args[0]);
    if (res == NULL) {
      snprintf(response->result, sizeof(response->result), "%d", 0);
      break;
    }
    if (res != NULL && res->expires_at != 0 && time(NULL) > res->expires_at) {
      // NOTE: htis is redundant, we are iterating over hmap twice
      delete_kv_in_hashmap(st_map, res->key);
      snprintf(response->result, sizeof(response->result), "%d", 0);
      break;
    }

    time_t expiration_at = time(NULL) + atoi(command->args[1]);
    res->expires_at = expiration_at;
    snprintf(response->result, sizeof(response->result), "%d", 1);
    break;
  }
  case Cmd_TTL: {
    Storage_hashmap_internal_ll *res =
        get_kv_in_hashmap(st_map, command->args[0]);
    if (res == NULL) {
      snprintf(response->result, sizeof(response->result), "%d", -2);
      break;
    }
    if (res != NULL && res->expires_at == 0) {
      snprintf(response->result, sizeof(response->result), "%d", -1);
      break;
    }
    if (res != NULL && time(NULL) > res->expires_at) {
      // NOTE: htis is redundant, we are iterating over hmap twice
      delete_kv_in_hashmap(st_map, res->key);
      snprintf(response->result, sizeof(response->result), "%d", -2);
      break;
    }

    time_t remaining_time = res->expires_at - time(NULL);
    snprintf(response->result, sizeof(response->result), "%d",
             (int)remaining_time);
    break;
  }
  default: {
    // do nothing
  }
  }
}

void dispatch_command(Storage_hashmap *st_map, Request *request,
                      Response *response) {
  switch (request->req_command_type) {
  case Cmd_Get: {
    Storage_hashmap_internal_ll *res = get_kv_in_hashmap(
        st_map, request->req_args + (1 * request->req_arg_size));
    if (res == NULL) {
      break;
    }
    if (res != NULL && res->expires_at != 0 && time(NULL) > res->expires_at) {
      // NOTE: htis is redundant, we are iterating over hmap twice
      delete_kv_in_hashmap(st_map, res->key);
      break;
    }

    strcpy(response->result, res->value);
    break;
  }
  case Cmd_Set: {
    // time_t expiration_at =
    //     request->req_arg_count == 5
    //         ? time(NULL) + atoi(request->req_args + (4 *
    //         request->req_arg_size)) : 0;

    char *temp;
    time_t expiration_at = (time_t)strtoll(
        request->req_args + (4 * request->req_arg_size), &temp, 10);
    set_kv_in_hashmap(st_map, request->req_args + (1 * request->req_arg_size),
                      request->req_args + (2 * request->req_arg_size),
                      expiration_at);
    break;
  }
  case Cmd_Del: {
    int deleted_count = 0;
    for (int i = 1; i < request->req_arg_count; i++) {
      char *key = request->req_args + (i * request->req_arg_size);
      bool is_deleted = delete_kv_in_hashmap(st_map, key);
      deleted_count += is_deleted ? 1 : 0;
    }
    snprintf(response->result, sizeof(response->result), "%d", deleted_count);
    break;
  }
  case Cmd_Expire: {
    Storage_hashmap_internal_ll *res = get_kv_in_hashmap(
        st_map, request->req_args + (1 * request->req_arg_size));
    if (res == NULL) {
      snprintf(response->result, sizeof(response->result), "%d", 0);
      break;
    }
    if (res != NULL && res->expires_at != 0 && time(NULL) > res->expires_at) {
      // NOTE: htis is redundant, we are iterating over hmap twice
      delete_kv_in_hashmap(st_map, res->key);
      snprintf(response->result, sizeof(response->result), "%d", 0);
      break;
    }

    time_t expiration_at =
        time(NULL) + atoi(request->req_args + (2 * request->req_arg_size));
    res->expires_at = expiration_at;
    snprintf(response->result, sizeof(response->result), "%d", 1);
    break;
  }
  case Cmd_TTL: {
    Storage_hashmap_internal_ll *res = get_kv_in_hashmap(
        st_map, request->req_args + (1 * request->req_arg_size));
    if (res == NULL) {
      snprintf(response->result, sizeof(response->result), "%d", -2);
      break;
    }
    if (res != NULL && res->expires_at == 0) {
      snprintf(response->result, sizeof(response->result), "%d", -1);
      break;
    }
    if (res != NULL && time(NULL) > res->expires_at) {
      // NOTE: htis is redundant, we are iterating over hmap twice
      delete_kv_in_hashmap(st_map, res->key);
      snprintf(response->result, sizeof(response->result), "%d", -2);
      break;
    }

    time_t remaining_time = res->expires_at - time(NULL);
    snprintf(response->result, sizeof(response->result), "%d",
             (int)remaining_time);
    break;
  }
  default: {
    // do nothing
  }
  }
}
