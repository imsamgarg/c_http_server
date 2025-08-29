#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server.c"

int main(int argc, char *argv[]) {
  struct addrinfo hints;
  struct addrinfo *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // Use my IP

  int error = getaddrinfo(NULL, "8080", &hints, &res);
  if (error != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
    return 1;
  }

  in_port_t port;

  if (res->ai_family == AF_INET) {
    char ipstr[INET_ADDRSTRLEN];

    inet_ntop(res->ai_family, &((struct sockaddr_in *)(res->ai_addr))->sin_addr,
              ipstr, sizeof(ipstr));

    port = ntohs(((struct sockaddr_in *)res->ai_addr)->sin_port);
    printf("Starting server on port: %d\n", port);
  } else {

    char ipstr[INET6_ADDRSTRLEN];
    inet_ntop(res->ai_family,
              &((struct sockaddr_in6 *)(res->ai_addr))->sin6_addr, ipstr,
              sizeof(ipstr));

    port = ntohs(((struct sockaddr_in6 *)res->ai_addr)->sin6_port);
    printf("Starting server on port: %d\n", port);
  }

  start_server(res);

  freeaddrinfo(res);
  return 0;
}
