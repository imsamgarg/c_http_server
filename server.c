#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "read_http.c"
int sfd = -1;

void handle_sigint(int sig) {
  if (sfd != -1) {
    close(sfd);
  }

  printf("Caught signal %d, exiting...\n", sig);
  exit(0);
}

void start_server(struct addrinfo *addr) {
  sfd = socket(addr->ai_family, addr->ai_socktype, 0);
  signal(SIGINT, handle_sigint);

  if (sfd == -1) {
    perror("socket");
    return;
  }
  int yes = 1;

  setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if (bind(sfd, addr->ai_addr, addr->ai_addrlen) < 0) {
    perror("bind");
    return;
  }

  if (listen(sfd, 5) < 0) {
    perror("listen");
    return;
  }

  printf("Listening\n");
  fflush(stdout);

  struct sockaddr *client_addr;
  socklen_t client_addr_size;

  while (1) {
    int fd = accept(sfd, client_addr, &client_addr_size);
    if (fd < 0) {
      perror("accept");
      continue;
    }

    struct http_request *req = malloc(sizeof(struct http_request));
    char *raw_request;
    int raw_request_size;

    read_http(fd, req, &raw_request, &raw_request_size);

    printf("%s %s [%d]\n", req->method, req->path, raw_request_size);
    fflush(stdout);

    char buf[128];
    sprintf(buf,
            "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: "
            "text/plain; charset=asci\r\n\r\n",
            raw_request_size - 1);

    send(fd, buf, strlen(buf), 0);
    send(fd, raw_request, raw_request_size, 0);

    // TODO: Better memory management
    free(raw_request);
    for (int i = 0; i < req->headers_count; i++) {
      free(req->headers[i].name);
      free(req->headers[i].value);
    }
    free(req->headers);
    free(req);

    close(fd);
  }

  close(sfd);
}
