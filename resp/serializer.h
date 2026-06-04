#ifndef RESP_SERIALIZER
#define RESP_SERIALIZER

#include "./../common/headers.h"

void cleanup_serializer(Serializer *serializer);
void init_serializer(Serializer *serializer);

void init_response(Response *response);
void cleanup_response(Response *response);
void send_response_only(int client_fd, Response *response,
                        Serializer *serializer);

#endif // !RESP_SERIALIZERk
