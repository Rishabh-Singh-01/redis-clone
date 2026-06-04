#include "./common/utils.c"
#include "./engine/aof.c"
#include "./engine/executor.c"
#include "./engine/hashmap.c"
#include "./resp/deserializer.c"
#include "./resp/resp.c"
#include "./resp/serializer.c"
#include "./tcp/sock.c"

int main() {
  int res = start_tcp_server(9999);
  return res;
}
