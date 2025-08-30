#include <string.h>

int is_upgrade_request(struct http_request *req);

void handle_ws_req(int fd, struct http_request *req);
