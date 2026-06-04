#ifndef COMMAND_DISPATCHER
#define COMMAND_DISPATCHER

#include "./../resp/req.h"
#include "./../resp/serializer.h"
#include "./../storage/hashmap.h"

bool validate_command(Command *command, Serializer *serializer);
void dispatch_command(Storage_hashmap *st_map, Request *request,
                      Response *response);
void execute_norm_command(Storage_hashmap *st_map, Response *response,
                          Command *command);

#endif // !COMMAND_DISPATCHER
