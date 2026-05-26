#include "./req.h"
#include "./deserializer.h"
#include "./serializer.h"
#include "command.h"

Request request;
Deserializer deserializer;
Serializer serializer;

void execute_resp(int client_fd, TCP_Server *tcp_server) {
  // TODO: Only works assuming each "request" will be containing complete
  // commands

  initialize_request(&request);
  initialize_deserializer(&deserializer);
  while ((deserializer.bytes_read_count = read(client_fd, deserializer.buffer,
                                               deserializer.buffer_size)) > 0) {
    execute_deserializer(&request, &deserializer);

    dispatch_command(&request);

    init_serializer(&serializer);
    send_response(client_fd, &request, &serializer);

    initialize_request(&request);
    initialize_deserializer(&deserializer);
    // TODO: add logic to free mems as well from init funcs
  }

  if (deserializer.bytes_read_count == 0) {
    printf("Info: Connection closed\n");
  } else {
    perror("Error: Issue with connectio during close\n");
  }
  close(client_fd);

  printf("Info: Connection Closed. Total Connections: %d\n",
         --tcp_server->concurrent_conn);
}
