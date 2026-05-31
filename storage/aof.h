#ifndef STORAGE_AOF
#define STORAGE_AOF

#include "./../resp/deserializer.h"

void append_to_aof(Request *request, Deserializer *deserializer);

#endif // !STORAGE_AOF
