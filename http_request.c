#include "http_request.h"
#include <stdio.h>
#include <string.h>

// INFO: Assuming the name to be always in lower_case
struct http_header *get_header(struct http_request *req, char *name) {
  for (int i = 0; i < req->headers_count; i++) {
    if (strcmp(req->headers[i].name, name) == 0) {
      return &req->headers[i];
    }
  }

  return NULL;
}

void print_request(struct http_request *req) {
  printf("Method: %s\n", req->method);
  printf("Path: %s\n", req->path);
  printf("Headers:\n");

  for (int i = 0; i < req->headers_count; i++) {
    printf("  %s: %s\n", req->headers[i].name, req->headers[i].value);
  }
}
