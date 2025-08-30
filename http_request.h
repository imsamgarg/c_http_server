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

struct http_header *get_header(struct http_request *req, char *name);

void print_request(struct http_request *req);
