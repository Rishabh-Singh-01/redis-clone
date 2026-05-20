#include "./resp/req.c"
#include "./tcp/sock.c"

int main() {
  int res = start_tcp_server(9999);
  return res;
}
