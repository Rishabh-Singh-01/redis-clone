#include "./resp.h"
#include "./../engine/executor.h"
#include "./../engine/hashmap.h"
#include "./../resp/deserializer.h"
#include "./../resp/serializer.h"

Req_Parser req_parser;
Command command;
Storage_hashmap st_map;
Response response;
Serializer serializer;

// TODO: start from here the aof impl
// void load_aof() {
//   init_hashmap(&st_map);
//
//   init_serializer(&serializer);
//   initialize_request(&request);
//   init_response(&response);
//   initialize_deserializer(&deserializer);
//
//   FILE *file_ptr = fopen("./obj/aof.txt", "r");
//   if (file_ptr == NULL) {
//     return;
//   }
//   char buffer[100];
//   memset(&buffer, '\0', 100);
//
//   while (fgets(buffer, sizeof(buffer), file_ptr) != NULL) {
//     int args_count = atoi(buffer + 1);
//     int total_lines_to_read = args_count * 2;
//     memcpy(deserializer.buffer + deserializer.buffer_itr_idx, buffer,
//            strlen(buffer));
//     deserializer.buffer_itr_idx += strlen(buffer);
//
//     for (int i = 0; i < total_lines_to_read; i++) {
//       memset(&buffer, '\0', 100);
//       if (fgets(buffer, sizeof(buffer), file_ptr) != NULL) {
//         memcpy(deserializer.buffer + deserializer.buffer_itr_idx, buffer,
//                strlen(buffer));
//         deserializer.buffer_itr_idx += strlen(buffer);
//       } else {
//         printf("[EOF reached prematurely inside a chunk]\n");
//         break;
//       }
//     }
//     deserializer.bytes_read_count = strlen(deserializer.buffer);
//     deserializer.buffer_itr_idx = 0;
//
//     execute_deserializer(&request, &deserializer);
//     dispatch_command(&st_map, &request, &response);
//
//     cleanup_serializer(&serializer);
//     cleanup_request(&request);
//     cleanup_response(&response);
//     cleanup_deserializer(&deserializer);
//     init_serializer(&serializer);
//     initialize_request(&request);
//     init_response(&response);
//     initialize_deserializer(&deserializer);
//
//     memset(&buffer, '\0', 100);
//   }
//
//   cleanup_request(&request);
//   cleanup_response(&response);
//   cleanup_serializer(&serializer);
//   cleanup_deserializer(&deserializer);
//
//   fclose(file_ptr);
// }

void init_resp_sm() {
  init_hashmap(&st_map);
  init_req_parser(&req_parser);
  init_command(&command);
  init_serializer(&serializer);
  init_response(&response);
}

void reset_resp_sm() {
  reset_req_parser(&req_parser);
  reset_command(&command);

  cleanup_serializer(&serializer);
  cleanup_response(&response);
  init_serializer(&serializer);
  init_response(&response);
}

void cleanup_resp_sm() {
  reset_req_parser(&req_parser);
  reset_command(&command);
  cleanup_serializer(&serializer);
  cleanup_response(&response);
  cleanup_hashmap(&st_map);
}

void execute_resp_new(int client_fd) {
  init_resp_sm();

  int bytes_read_count = 0;
  while ((bytes_read_count =
              read(client_fd, req_parser.buffer, req_parser.buffer_size)) > 0) {
    exec_req_parser(&req_parser, &command);
    bool is_valid_cmd = validate_command(&command, &serializer);
    if (is_valid_cmd) {
      normalize_command(&command);
      execute_norm_command(&st_map, &response, &command);
    }
    send_response_only(client_fd, &response, &serializer);

    reset_resp_sm();
  }

  cleanup_resp_sm();

  if (bytes_read_count != 0) {
    perror("Error: Issue with connectio during close\n");
  }
  close(client_fd);
}
