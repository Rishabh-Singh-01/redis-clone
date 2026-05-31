#ifndef RESP_RES
#define RESP_RES

#include "./../common/headers.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void initialize_response(Response *response);
void cleanup_response(Response *response);

#endif // !RESP_RES
