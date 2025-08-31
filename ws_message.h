#define WS_CONTINUATION_FRAME 0x1
#define WS_TEXT_FRAME 0x1
#define WS_BINARY_FRAME 0x2
#define WS_CLOSE_FRAME 0x8
#define WS_PING_FRAME 0x9
#define WS_PONG_FRAME 0xA

struct ws_message {
  //  0x0 -> continuation frame
  //  0x1 → text frame
  //  0x2 → binary frame
  //  0x8 → connection close
  //  0x9 → ping
  //  0xA → pong
  int opcode;
  int payload_length;
  char *payload;
};

int parse_ws_message(char *buf, struct ws_message *msg);
