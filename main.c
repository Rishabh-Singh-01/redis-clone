#include "./common/headers.h"
#include "./tcp/sock.c"
#include "tcp/sock.h"

int main() {
  int res = start_tcp_server(9999);
  return res;
}
