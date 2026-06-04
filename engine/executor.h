#ifndef COMMAND_DISPATCHER
#define COMMAND_DISPATCHER

#include "./../common/headers.h"

void init_command(Command *command);
void reset_command(Command *command);
bool validate_command(Command *command, Serializer *serializer);
void dispatch_command(Storage_hashmap *st_map, Request *request,
                      Response *response);
void execute_norm_command(Storage_hashmap *st_map, Response *response,
                          Command *command);
void normalize_command(Command *command);

#endif // !COMMAND_DISPATCHER
