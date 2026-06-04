#ifndef STORAGE_HASHMAP
#define STORAGE_HASHMAP

#include "./../common/headers.h"

void init_hashmap(Storage_hashmap *st_map);
void cleanup_hashmap(Storage_hashmap *st_map);
Storage_hashmap_internal_ll *get_kv_in_hashmap(Storage_hashmap *st_map,
                                               char *key);
void set_kv_in_hashmap(Storage_hashmap *st_map, char *key, char *value,
                       time_t expiration_at);
bool delete_kv_in_hashmap(Storage_hashmap *st_map, char *key);

#endif // !STORAGE_HASHMAP
