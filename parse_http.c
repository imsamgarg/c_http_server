#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "http_request.h"
#include "parse_http.h"
#include "utils.h"

// GET /hello/world HTTP/1.1\r\n
// Host: example.com\r\n
// User-Agent: curl/8.5.0\r\n
// Accept: */*\r\n
// \r\n

char *read_block(char *buffer, char *start, char ch, char *name) {
  char *ptr = strchr(start, ' ');
  memcpy(buffer, start, ptr - start);
  buffer[ptr - start] = 0;

  // printf("%s: %s\n", name, buffer);
  // fflush(stdout);

  return ptr + 1;
}

int parse_http(char *request, size_t size, http_request *req, arena *a) {
  char *ptr = read_block(req->method, request, ' ', "Method");
  ptr = read_block(req->path, ptr, ' ', "Path");
  ptr = strchr(ptr, '\n') + 1;

  // INFO: Only parsing 20 headers at max
  http_header *headers = allocate(20 * sizeof(http_header), a);
  memset(headers, 0, 20 * sizeof(http_header));
  req->headers = headers;
  req->headers_count = 0;

  http_header *header = headers;

  while (*ptr) {
    char *newptr = strchr(ptr, ':');
    int size = newptr - ptr;
    char *name_buffer = allocate(size + 1, a);
    memcpy(name_buffer, ptr, size);
    name_buffer[size] = 0;

    ptr = newptr + 2;
    newptr = strchr(ptr, '\r');
    size = newptr - ptr;
    char *value_buffer = allocate(size + 1, a);
    memcpy(value_buffer, ptr, size);
    value_buffer[size] = 0;

    header->name = name_buffer;
    lower_case(header->name);

    header->value = value_buffer;
    // fflush(stdout);
    header++;
    req->headers_count++;

    ptr = newptr + 2;

    if (*ptr == 0) {
      break;
    }
    if (*ptr == '\r') {
      // INFO: end of the headers
      break;
    }
  }

  return 0;
}
