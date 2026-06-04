#ifndef RESP_SERIALIZER
#define RESP_SERIALIZER

#include "./../common/headers.h"

void cleanup_serializer(Serializer *serializer);
void init_serializer(Serializer *serializer);
void send_response(int client_fd, Request *request, Response *response,
                   Serializer *serializer);
void send_response_new(int client_fd, Command *command, Response *response,
                       Serializer *serializer);

void init_response(Response *response);
void cleanup_response(Response *response);
void generate_response(Command *command, Response *response,
                       Serializer *serializer);
void send_response_only(int client_fd, Serializer *serializer);

#endif // !RESP_SERIALIZERk
