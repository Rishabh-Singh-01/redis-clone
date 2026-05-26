#include "./command.h"

void extract_command(Request *request) {
  if (strcasecmp(request->req_args, "PING") == 0) {
    request->req_command_type = Req_Ping;
  } else if (strcasecmp(request->req_args, "ECHO") == 0) {
    request->req_command_type = Req_Echo;
  } else {
    request->req_command_type = Req_Invalid;
  }
}
