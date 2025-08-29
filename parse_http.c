#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

struct http_header {
  char *name;
  char *value;
};

struct http_request {
  char method[6];
  // INFO: Supporting only 256 byte path for now.
  char path[256];
  struct http_header *headers;
  int headers_count;
};

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

int parse_http(char *request, size_t size, struct http_request *req) {
  char *ptr = read_block(req->method, request, ' ', "Method");
  ptr = read_block(req->path, ptr, ' ', "Path");
  ptr = strchr(ptr, '\n') + 1;

  // INFO: Only parsing 20 headers at max
  struct http_header *headers = malloc(20 * sizeof(struct http_header));
  memset(headers, 0, 20 * sizeof(struct http_header));
  req->headers = headers;

  struct http_header *header = headers;

  while (*ptr) {
    char *newptr = strchr(ptr, ':');
    int size = newptr - ptr;
    char *name_buffer = malloc(size + 1);
    memcpy(name_buffer, ptr, size);
    name_buffer[size] = 0;

    ptr = newptr + 2;
    newptr = strchr(ptr, '\r');
    size = newptr - ptr;
    char *value_buffer = malloc(size + 1);
    memcpy(value_buffer, ptr, size);
    value_buffer[size] = 0;

    header->name = name_buffer;
    header->value = value_buffer;
    // printf("%s : %s\n", name_buffer, value_buffer);
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
