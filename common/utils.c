#include <stdio.h>
void print_hex(const char *s) {
  while (*s) {
    // %02x ensures at least 2 digits with leading zeros
    printf("%02x ", (unsigned char)*s++);
  }
  printf("\n");
}
