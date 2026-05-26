#ifndef RESP_RES
#define RESP_RES

#include "./../common/headers.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void execute_resp(int client_fd, TCP_Server *tcp_server);

#endif // !RESP_RES
