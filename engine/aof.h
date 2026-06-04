#ifndef STORAGE_AOF
#define STORAGE_AOF

#include "./../common/headers.h"

void append_to_aof(Request *request, Deserializer *deserializer);

#endif // !STORAGE_AOF
