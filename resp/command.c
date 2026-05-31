#include "./command.h"

void extract_command(Request *request) {
  if (strcasecmp(request->req_args, "PING") == 0) {
    request->req_command_type = Cmd_Ping;
  } else if (strcasecmp(request->req_args, "ECHO") == 0) {
    request->req_command_type = Cmd_Echo;
  } else if (strcasecmp(request->req_args, "GET") == 0) {
    request->req_command_type = Cmd_Get;
  } else if (strcasecmp(request->req_args, "SET") == 0) {
    request->req_command_type = Cmd_Set;
  } else if (strcasecmp(request->req_args, "DEL") == 0) {
    request->req_command_type = Cmd_Del;
  } else if (strcasecmp(request->req_args, "Expire") == 0) {
    request->req_command_type = Cmd_Expire;
  } else if (strcasecmp(request->req_args, "TTL") == 0) {
    request->req_command_type = Cmd_TTL;
  } else if (strcasecmp(request->req_args, "ECHO") == 0) {
    request->req_command_type = Cmd_Echo;
  } else {
    request->req_command_type = Cmd_Invalid;
  }
}
