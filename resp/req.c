#include "./req.h"
#include "./../command/dispatcher.h"
#include "./../storage/aof.h"
#include "./../storage/hashmap.h"
#include "./deserializer.h"
#include "./res.h"
#include "./serializer.h"
#include <stdio.h>
#include <string.h>

Request request;
Response response;
Deserializer deserializer;
Serializer serializer;
Storage_hashmap st_map;

// TODO: start from here the aof impl
void load_aof() {
  init_hashmap(&st_map);

  init_serializer(&serializer);
  initialize_request(&request);
  initialize_response(&response);
  initialize_deserializer(&deserializer);

  FILE *file_ptr = fopen("./obj/aof.txt", "r");
  if (file_ptr == NULL) {
    return;
  }
  char buffer[100];
  memset(&buffer, '\0', 100);

  while (fgets(buffer, sizeof(buffer), file_ptr) != NULL) {
    int args_count = atoi(buffer + 1);
    int total_lines_to_read = args_count * 2;
    memcpy(deserializer.buffer + deserializer.buffer_itr_idx, buffer,
           strlen(buffer));
    deserializer.buffer_itr_idx += strlen(buffer);

    for (int i = 0; i < total_lines_to_read; i++) {
      memset(&buffer, '\0', 100);
      if (fgets(buffer, sizeof(buffer), file_ptr) != NULL) {
        memcpy(deserializer.buffer + deserializer.buffer_itr_idx, buffer,
               strlen(buffer));
        deserializer.buffer_itr_idx += strlen(buffer);
      } else {
        printf("[EOF reached prematurely inside a chunk]\n");
        break;
      }
    }

    printf("Deserializer line: %s\n", deserializer.buffer);
    cleanup_serializer(&serializer);
    cleanup_request(&request);
    cleanup_response(&response);
    cleanup_deserializer(&deserializer);
    init_serializer(&serializer);
    initialize_request(&request);
    initialize_response(&response);
    initialize_deserializer(&deserializer);

    execute_deserializer(&request, &deserializer);
    dispatch_command(&st_map, &request, &response);

    cleanup_serializer(&serializer);
    cleanup_request(&request);
    cleanup_response(&response);
    cleanup_deserializer(&deserializer);
    init_serializer(&serializer);
    initialize_request(&request);
    initialize_response(&response);
    initialize_deserializer(&deserializer);

    memset(&buffer, '\0', 100);
  }

  cleanup_request(&request);
  cleanup_response(&response);
  cleanup_serializer(&serializer);
  cleanup_deserializer(&deserializer);

  fclose(file_ptr);
}

void execute_resp(int client_fd, TCP_Server *tcp_server) {
  // TODO: Only works assuming each "request" will be containing complete
  // commands

  // init_hashmap(&st_map);

  init_serializer(&serializer);
  initialize_request(&request);
  initialize_response(&response);
  initialize_deserializer(&deserializer);
  while ((deserializer.bytes_read_count = read(client_fd, deserializer.buffer,
                                               deserializer.buffer_size)) > 0) {
    execute_deserializer(&request, &deserializer);
    append_to_aof(&request, &deserializer);
    dispatch_command(&st_map, &request, &response);
    send_response(client_fd, &request, &response, &serializer);

    cleanup_serializer(&serializer);
    cleanup_request(&request);
    cleanup_response(&response);
    cleanup_deserializer(&deserializer);
    init_serializer(&serializer);
    initialize_request(&request);
    initialize_response(&response);
    initialize_deserializer(&deserializer);
  }

  cleanup_request(&request);
  cleanup_response(&response);
  cleanup_serializer(&serializer);
  cleanup_deserializer(&deserializer);

  if (deserializer.bytes_read_count == 0) {
    printf("Info: Connection closed\n");
  } else {
    perror("Error: Issue with connectio during close\n");
  }
  close(client_fd);

  printf("Info: Connection Closed. Total Connections: %d\n",
         --tcp_server->concurrent_conn);
}
