#ifndef COMMAND_DISPATCHER
#define COMMAND_DISPATCHER

#include "./../resp/req.h"
#include "./../storage/hashmap.h"

void dispatch_command(Storage_hashmap *st_map, Request *request,
                      Response *response);
void execute_norm_command(Storage_hashmap *st_map, Response *response,
                          Command *command);

#endif // !COMMAND_DISPATCHER
