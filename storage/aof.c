#include "./aof.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void append_to_aof(Request *request, Deserializer *deserializer) {
  // TODO: currently we are appending at each cmd, very slow
  if (!(request->req_command_type == Cmd_Set ||
        request->req_command_type == Cmd_Del ||
        request->req_command_type == Cmd_Expire)) {
    return;
  }

  FILE *file_ptr = fopen("./obj/aof.txt", "a");
  if (file_ptr == NULL) {
    perror("Error opening file");
    return;
  }
  size_t data_size = strlen(deserializer->buffer);
  size_t bytes_written = fwrite(deserializer->buffer, 1, data_size, file_ptr);
  if (bytes_written < data_size) {
    printf("Warning: Did not write full buffer!\n");
  };
  fclose(file_ptr);
}
