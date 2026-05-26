#include "./command/dispatcher.c"
#include "./resp/command.c"
#include "./resp/deserializer.c"
#include "./resp/req.c"
#include "./resp/serializer.c"
#include "./storage/hashmap.c"
#include "./tcp/sock.c"

int main() {
  int res = start_tcp_server(9999);
  return res;
}
