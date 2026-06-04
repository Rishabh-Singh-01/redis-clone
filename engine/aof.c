#include "./aof.h"

// void append_to_aof(Request *request, Deserializer *deserializer) {
//   // TODO: currently we are appending at each cmd, very slow
//   if (!(request->req_command_type == Cmd_Set ||
//         request->req_command_type == Cmd_Del ||
//         request->req_command_type == Cmd_Expire)) {
//     return;
//   }
//
//   FILE *file_ptr = fopen("./obj/aof.txt", "a");
//   if (file_ptr == NULL) {
//     perror("Error opening file");
//     return;
//   }
//
//   if (request->req_command_type == Cmd_Set) {
//     time_t expiration_at =
//         request->req_arg_count == 5
//             ? time(NULL) + atoi(request->req_args + (4 *
//             request->req_arg_size)) : 0;
//     char temp[100];
//     memset(&temp, '\0', 100);
//     strcpy(temp, deserializer->buffer);
//     // NOTE: wont work for key/value of type ex
//     // this is the worst parser ever, can't be called parser
//     // very werid, expiration converter, i literally hardcoded everything
//     char *pos = strstr(temp, "ex");
//     if (pos != NULL) {
//       int sub_str = pos - temp;
//       *(deserializer->buffer + sub_str - 3) = '4';
//       strcpy(deserializer->buffer + sub_str, "exat\r\n$");
//       memset(&temp, '\0', 100);
//       sprintf(temp, "%lld", (long long)expiration_at);
//       int expiration_at_len = strlen(temp);
//       sprintf(deserializer->buffer + sub_str + 7, "%lld\r\n",
//               (long long)expiration_at_len);
//       memset(&temp, '\0', 100);
//       sprintf(temp, "%d", expiration_at_len);
//       int expiration_at_len_len = strlen(temp);
//       memset(&temp, '\0', 100);
//       sprintf(temp, "%lld\r\n", (long long)expiration_at);
//       printf("this is the expiration_at%lld\n", (long long)expiration_at);
//       printf("sprit temp: %s\n", temp);
//       strcpy(deserializer->buffer + sub_str + 7 + expiration_at_len_len + 2,
//              temp);
//       printf("deser buffer: %s\n", deserializer->buffer);
//     } else {
//       // This means the set x y does not contain any expiry, so append 0 to
//       it int sub_str = strlen(deserializer->buffer);
//       strcpy(deserializer->buffer + sub_str, "$4\r\nexat\r\n$1\r\n0\r\n");
//     }
//   }
//
//   size_t data_size = strlen(deserializer->buffer);
//   size_t bytes_written = fwrite(deserializer->buffer, 1, data_size,
//   file_ptr);
//
//   if (bytes_written < data_size) {
//     printf("Warning: Did not write full buffer!\n");
//   };
//   fclose(file_ptr);
// }
