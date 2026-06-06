#include "./eventloop.h"
#include "./../resp/resp.h"
#include "./sock.h"
#include <stdio.h>

int run_event_loop(TCP_Server *tcp_server) {

  int kq = kqueue();
  if (kq == -1) {
    perror("Unable to Kqueue\n");
    exit(1);
  }

  struct kevent change;
  EV_SET(&change, tcp_server->socket_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  kevent(kq, &change, 1, NULL, 0, NULL);

  struct kevent events[128];
  while (1) {
    int n = kevent(kq, NULL, 0, events, 128, NULL);

    for (int i = 0; i < n; i++) {
      if (events[i].ident == tcp_server->socket_fd) {
        int client_fd = accept(tcp_server->socket_fd, NULL, NULL);
        printf("Accepted Conn: %d\n", client_fd);
        struct kevent ev;
        EV_SET(&ev, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        kevent(kq, &ev, 1, NULL, 0, NULL);
      } else {
        execute_resp_new(events->ident);
      }
    }
  }

  return 0;
}
