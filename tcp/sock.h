#ifndef TCP_SOCK
#define TCP_SOCK
#include "./../common/headers.h"

int start_tcp_server(int port, TCP_Server *tcp_server);
int accept_and_read_conn(TCP_Server *tcp_server);
int accept_conn(TCP_Server *tcp_server);

#endif /* ifndef TCP_SOCK */
