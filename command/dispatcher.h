#ifndef COMMAND_DISPATCHER
#define COMMAND_DISPATCHER

#include "./../resp/req.h"
#include "./../storage/hashmap.h"

void dispatch_command(Storage_hashmap *st_map, Request *request,
                      Response *response);

#endif // !COMMAND_DISPATCHER
