#ifndef STORAGE_HASHMAP
#define STORAGE_HASHMAP

#include "./../common/utils.h"
#include "stdlib.h"
#include <stdbool.h>
#include <time.h>

typedef struct STORAGE_HASHMAP_INTERNAL_LL_STRUCT {
  char *key;
  char *value;
  time_t expires_at;
  struct STORAGE_HASHMAP_INTERNAL_LL_STRUCT *next;
} Storage_hashmap_internal_ll;

typedef struct STORAGE_HASHMAP_STRUCT {
  int buckets_count;
  Storage_hashmap_internal_ll **bucket;
} Storage_hashmap;

void init_hashmap(Storage_hashmap *st_map);
Storage_hashmap_internal_ll *get_kv_in_hashmap(Storage_hashmap *st_map,
                                               char *key);
void set_kv_in_hashmap(Storage_hashmap *st_map, char *key, char *value,
                       time_t expiration_at);

bool delete_kv_in_hashmap(Storage_hashmap *st_map, char *key);
#endif // !STORAGE_HASHMAP
