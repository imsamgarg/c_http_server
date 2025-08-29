#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "parse_http.c"

int read_http(int fd, struct http_request *req, char **raw_request,
              int *raw_request_size) {
  const int size = 4096;
  char buf[size];
  char *request = (char *)malloc(size * 4);

  int n = 0, ptr = 0, request_size = size * 4;

  while ((n = recv(fd, buf, sizeof buf, 0)) > 0) {
    if ((ptr + n) <= request_size) {
      strncpy(request + ptr, buf, n);
    } else {
      request_size *= 2;
      request = reallocarray(request, sizeof(char), request_size);
      strncpy(request + ptr, buf, n);
    }

    ptr += n;

    if (n < size) {
      break;
    }
  }

  if (n == 0) {
    perror("read");
    return errno;
  }

  request[ptr] = '\0';

  FILE *file = fopen("request.html", "w+");

  fprintf(file, "%s", request);

  fclose(file);

  *raw_request = request;
  *raw_request_size = ptr;

  // free(request);

  return parse_http(request, ptr, req);
}
