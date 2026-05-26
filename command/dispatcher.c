#include "./dispatcher.h"

void dispatch_command(Request *request) {
  switch (request->req_command_type) {
  case Req_Get: {
    // do nothing
  }
  case Req_Set: {
    // do nothing
  }
  default: {
    // do nothing
  }
  }
}
