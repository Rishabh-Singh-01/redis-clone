#include "./deserializer.h"

char CRLF[2] = {'\r', '\n'};

void initialize_request(Request *request) {
  request->req_arg_count = 10;
  request->req_arg_size = 10;
  request->req_arg_idx = 0;
  request->req_command_type = Cmd_Invalid;
  request->req_args = (char *)malloc(sizeof(char) * request->req_arg_count *
                                     request->req_arg_size);
  memset(request->req_args, '\0',
         sizeof(char) * request->req_arg_count * request->req_arg_size);
}

void cleanup_request(Request *request) { free(request->req_args); }

void cleanup_deserializer(Deserializer *deserializer) {
  deserializer->bytes_read_count = 0;
  deserializer->read_buffer_offset_idx = 0;
  deserializer->crlf_visited_count = 0;
  deserializer->cur_arg_consumed_count = 0;
  deserializer->cur_arg_size = 0;
  deserializer->buffer_size = 1024;
  deserializer->buffer_itr_idx = 0;
  free(deserializer->buffer);
}

void initialize_deserializer(Deserializer *deserializer) {
  deserializer->bytes_read_count = 0;
  deserializer->read_buffer_offset_idx = 0;

  deserializer->crlf_visited_count = 0;
  deserializer->cur_arg_consumed_count = 0;
  deserializer->cur_arg_size = 0;
  deserializer->buffer_size = 1024;
  deserializer->buffer = (char *)malloc(sizeof(char) * 1024);
  memset(deserializer->buffer, '\0', sizeof(char) * 1024);
  deserializer->buffer_itr_idx = 0;
}

void execute_deserializer(Request *request, Deserializer *deserializer) {
  while (deserializer->buffer_itr_idx < deserializer->bytes_read_count) {

    bool is_idx_on_req_header = deserializer->crlf_visited_count == 0;
    bool is_idx_on_crlf =
        deserializer->buffer[deserializer->buffer_itr_idx] == CRLF[0] &&
        deserializer->buffer[deserializer->buffer_itr_idx + 1] == CRLF[1];
    bool is_idx_on_payload_header = deserializer->crlf_visited_count > 0 &&
                                    deserializer->crlf_visited_count % 2 != 0;
    bool is_idx_on_payload = deserializer->crlf_visited_count > 0 &&
                             deserializer->crlf_visited_count % 2 == 0;
    bool is_payload_consumed =
        deserializer->cur_arg_consumed_count == deserializer->cur_arg_size;

    if (!is_idx_on_crlf && (is_idx_on_req_header || is_idx_on_payload_header)) {
      deserializer->buffer_itr_idx++;
      continue;
    }

    if (is_idx_on_req_header && is_idx_on_crlf) {
      assert(deserializer->buffer[deserializer->buffer_itr_idx - 2] == '*');
      assert(deserializer->buffer[deserializer->buffer_itr_idx - 1] > '0');
      request->req_arg_count =
          deserializer->buffer[deserializer->buffer_itr_idx - 1] - '0';
      deserializer->buffer_itr_idx += 2;
      deserializer->crlf_visited_count++;
      continue;
    }

    if (is_idx_on_payload_header && is_idx_on_crlf) {
      assert(deserializer->buffer[deserializer->buffer_itr_idx - 2] == '$');
      assert(deserializer->buffer[deserializer->buffer_itr_idx - 1] > '0');
      deserializer->cur_arg_size =
          deserializer->buffer[deserializer->buffer_itr_idx - 1] - '0';
      deserializer->buffer_itr_idx += 2;
      deserializer->crlf_visited_count++;
      continue;
    }

    if (is_idx_on_payload && !is_payload_consumed) {
      int cur_arg_itr_idx = request->req_arg_idx * request->req_arg_size +
                            deserializer->cur_arg_consumed_count;
      request->req_args[cur_arg_itr_idx] =
          deserializer->buffer[deserializer->buffer_itr_idx];
      deserializer->cur_arg_consumed_count++;
      deserializer->buffer_itr_idx++;
      continue;
    }

    if (is_idx_on_payload && is_payload_consumed) {
      extract_command(request);

      request->req_arg_idx++;
      deserializer->buffer_itr_idx += 2;
      deserializer->crlf_visited_count++;
      deserializer->cur_arg_size = 0;
      deserializer->cur_arg_consumed_count = 0;
    }
  }
}
