#include "./res.h"

void initialize_response(Response *response) {
  response->res_command_type = Cmd_Invalid;
  response->res_size = 100;
  response->result_size = 0;
  response->result = (char *)malloc(sizeof(char) * response->res_size);
  memset(response->result, '\0', sizeof(char) * response->res_size);
}

void cleanup_response(Response *response) {
  response->res_command_type = Cmd_Invalid;
  response->res_size = 100;
  response->result_size = 0;
  free(response->result);
}
