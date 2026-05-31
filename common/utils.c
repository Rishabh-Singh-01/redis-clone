#include "./utils.h"
void print_hex(const char *s) {
  while (*s) {
    // %02x ensures at least 2 digits with leading zeros
    printf("%02x ", (unsigned char)*s++);
  }
  printf("\n");
}

unsigned long djb2_hash(unsigned char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  return hash;
}

int get_hash_in_range(unsigned char *str, int n) {
  return (int)(djb2_hash(str) % n);
}
