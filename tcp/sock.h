#ifndef TCP_SOCK
#define TCP_SOCK
#include "./../common/headers.h"
#include "./../resp/req.h"
#include <arpa/inet.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int start_tcp_server(int port);

#endif /* ifndef TCP_SOCK */
