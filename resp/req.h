#ifndef RESP_REQ
#define RESP_REQ

#include "./../common/headers.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void deserialize_request(int client_fd, TCP_Server *tcp_server);

#endif // !RESP_REQ
