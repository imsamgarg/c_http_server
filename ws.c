#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_request.h"
#include "ws_message.h"

int is_upgrade_request(struct http_request *req) {
  // Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
  // Sec-WebSocket-Version: 13
  // Upgrade: websocket
  // Connection: Upgrade

  struct http_header *upgrade_head = get_header(req, "upgrade");
  printf("Checking upgrade header\n");
  if (upgrade_head == NULL || strcmp(upgrade_head->value, "websocket") != 0) {
    return 0;
  }

  printf("Found upgrade header\n");

  struct http_header *con_head = get_header(req, "connection");
  if (con_head == NULL || strcmp(con_head->value, "Upgrade") != 0) {
    return 0;
  }
  printf("Found connection header\n");

  struct http_header *sec_key_head = get_header(req, "sec-websocket-key");
  if (sec_key_head == NULL) {
    return 0;
  }
  printf("Found sec-websocket-key header\n");

  struct http_header *sec_version_head =
      get_header(req, "sec-websocket-version");
  if (sec_version_head == NULL || strcmp(sec_version_head->value, "13") != 0) {
    return 0;
  }
  printf("Found sec-websocket-version header\n");

  return 1;
}

int generate_ws_accept_key(char *client_key, char *key) {
  const char guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  size_t client_key_size = strlen(client_key);

  char buf[128] = {0};
  sprintf(buf, "echo -n \"%s%s\" | sha1sum", client_key, guid);

  // INFO: For now using method to calculate sha, instead create a sha function
  FILE *fp = popen(buf, "r");
  if (fp == NULL) {
    return -1;
  }

  fgets(key, 41, fp);
  fclose(fp);

  // INFO: Create a base64 function instead of using method
  sprintf(buf, "echo -n \"%s\" | xxd -r -p | base64", key);
  fp = popen(buf, "r");
  if (fp == NULL) {
    return -1;
  }

  fgets(key, 256, fp);
  fclose(fp);
  return 0;
}

void handle_ws_req(int fd, struct http_request *req) {

  char base64_sha_key[128] = {0};
  if (generate_ws_accept_key(get_header(req, "sec-websocket-key")->value,
                             base64_sha_key) < 0) {
    fprintf(stderr, "Failed to generate sha1sum");
    close(fd);
    return;
  }

  printf("key len: %ld\n", strlen(base64_sha_key));

  char buf[256];

  sprintf(buf,
          "HTTP/1.1 101 Switching Protocols\r\n"
          "Upgrade: websocket\r\n"
          "Connection: Upgrade\r\n"
          "Sec-WebSocket-Accept: %s\r\n\r\n",
          base64_sha_key);

  // WARN: For some reason there's an extra newline character at the last of the
  // string, inspect it
  send(fd, buf, strlen(buf) - 1, 0);

  while (1) {
    char buf[256] = {0};
    if (recv(fd, buf, sizeof(buf), 0) < 0) {
      perror("recv");
      close(fd);
      return;
    }

    struct ws_message msg;
    parse_ws_message(buf, &msg);

    switch (msg.opcode) {
    case WS_CLOSE_FRAME:
      break;
    case WS_TEXT_FRAME:
      printf("Message Received: %s\n", msg.payload);
      // TODO: Echo the message back with proper ws framing
      // send(fd, msg.payload, msg.payload_length, 0);
      break;
    case WS_BINARY_FRAME:
      printf("Binary Message Received: %d\n", msg.payload_length);
      break;
    case WS_PING_FRAME:
      break;
    case WS_PONG_FRAME:
      break;
    }
  }
}
