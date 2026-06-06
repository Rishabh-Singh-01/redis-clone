#ifndef RESP_DESERIALIZER
#define RESP_DESERIALIZER

#include "./../common/headers.h"

void init_command(Command *command);
void init_req_parser(Req_Parser *req_parser);
void exec_req_parser(Req_Parser *req_parser, Command *command);
void reset_req_parser(Req_Parser *req_parser);
void print_command(Command *command);
void reset_command(Command *command);
char *cmd_to_req(Command *command);

#endif // !
