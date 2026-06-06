#include "./common/utils.c"
#include "./engine/aof.c"
#include "./engine/executor.c"
#include "./engine/hashmap.c"
#include "./resp/deserializer.c"
#include "./resp/resp.c"
#include "./resp/serializer.c"
#include "./tcp/eventloop.c"
#include "./tcp/sock.c"
#include "tcp/eventloop.h"

int main() {
  TCP_Server tcp_server;
  int res = start_tcp_server(9999, &tcp_server);
  run_event_loop(&tcp_server);

  return res;
}
