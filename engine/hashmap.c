#include "./hashmap.h"

// TODO: a bad impl of hashmap (make it better)

void debug_hashmap(Storage_hashmap *st_map) {
  printf("Inside here\n");
  for (int i = 0; i < st_map->buckets_count; i++) {
    printf("Inside hashmap: %d\n", i);
    Storage_hashmap_internal_ll *ll_head = st_map->bucket[i];
    Storage_hashmap_internal_ll *ll_itr = ll_head;
    while (ll_itr != NULL) {
      printf("Value: %s\n", ll_itr->value);
      ll_itr = ll_itr->next;
    }
  }
}

void init_hashmap(Storage_hashmap *st_map) {
  st_map->buckets_count = 10;
  st_map->bucket = (Storage_hashmap_internal_ll **)malloc(
      sizeof(Storage_hashmap_internal_ll *) * st_map->buckets_count);
  for (int i = 0; i < st_map->buckets_count; i++) {
    (st_map->bucket)[i] = NULL;
  }
}

void cleanup_hashmap(Storage_hashmap *st_map) {
  for (int i = 0; i < st_map->buckets_count; i++) {
    free(st_map->bucket[i]);
  }
  free(st_map->bucket);
}

Storage_hashmap_internal_ll *get_kv_in_hashmap(Storage_hashmap *st_map,
                                               char *key) {
  int bucket_num =
      get_hash_in_range((unsigned char *)key, st_map->buckets_count);
  Storage_hashmap_internal_ll *ll_head = st_map->bucket[bucket_num];
  Storage_hashmap_internal_ll *ll_itr = ll_head;
  while (ll_itr != NULL) {
    if (strcmp(key, ll_itr->key) == 0) {
      return ll_itr;
    }
    ll_itr = ll_itr->next;
  }
  return NULL;
}
void set_kv_in_hashmap(Storage_hashmap *st_map, char *key, char *value,
                       time_t expiration_at) {
  int bucket_num =
      get_hash_in_range((unsigned char *)key, st_map->buckets_count);

  Storage_hashmap_internal_ll *prev_set_node = get_kv_in_hashmap(st_map, key);
  if (prev_set_node != NULL) {
    // TODO: what would happen if cur value is longer than prev value
    printf("will it do inside\n");
    strcpy(prev_set_node->value, value);
    prev_set_node->expires_at = expiration_at;
    return;
  }

  Storage_hashmap_internal_ll *ll_node = (Storage_hashmap_internal_ll *)malloc(
      sizeof(Storage_hashmap_internal_ll) * 1);
  ll_node->next = NULL;
  ll_node->key = (char *)malloc(sizeof(char) * strlen(key));
  ll_node->value = (char *)malloc(sizeof(char) * strlen(value));
  ll_node->expires_at = expiration_at;
  strcpy(ll_node->key, key);
  strcpy(ll_node->value, value);

  Storage_hashmap_internal_ll *ll_itr = st_map->bucket[bucket_num];
  if (ll_itr == NULL) {
    st_map->bucket[bucket_num] = ll_node;
  } else {
    while (ll_itr->next != NULL) {
      ll_itr = ll_itr->next;
    }
    ll_itr->next = ll_node;
  }
}

bool delete_kv_in_hashmap(Storage_hashmap *st_map, char *key) {
  int bucket_num =
      get_hash_in_range((unsigned char *)key, st_map->buckets_count);

  Storage_hashmap_internal_ll *ll_head = st_map->bucket[bucket_num];
  Storage_hashmap_internal_ll *ll_prev_itr = NULL;
  Storage_hashmap_internal_ll *ll_itr = ll_head;

  while (ll_itr != NULL) {
    if (strcmp(key, ll_itr->key) == 0) {
      if (ll_itr == ll_head) {
        st_map->bucket[bucket_num] = ll_itr == NULL ? NULL : ll_itr->next;
      } else {
        ll_prev_itr->next = ll_itr->next;
      }

      free(ll_itr->key);
      free(ll_itr->value);
      free(ll_itr);
      return true;
    }

    ll_prev_itr = ll_itr;
    ll_itr = ll_itr->next;
  }

  return false;
}
