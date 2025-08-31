//   0               1               2               3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//  +-+-+-+-+-------+-+-------------+-------------------------------+
//  |F|R|R|R| opcode|M| Payload len |    Extended length (optional) |
//  |I|S|S|S|  (4)  |A|     (7)     |   (16/64 bits if needed)      |
//  |N|V|V|V|       |S|             |                               |
//  +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
//  |     Masking key (32 bits, if MASK set)                        |
//  +---------------------------------------------------------------+
//  |                     Payload Data (x bytes)                    |
//  +---------------------------------------------------------------+

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "ws_message.h"

int ws_frame_message(struct ws_message msg, struct ws_frame *frame) {
  char *buf = malloc(msg.payload_length + 10); // max 10 bytes for the header

  printf("Framing message of length: %d\n", msg.payload_length);

  buf[0] = 0b10000000 | msg.opcode;
  int ptr = 0;
  if (msg.payload_length <= 125) {
    buf[1] = (char)msg.payload_length;
    ptr = 2;
  } else if (msg.payload_length == 126) {
    uint16_t len = msg.payload_length;
    buf[1] = 126;
    buf[2] = len >> 8;
    buf[3] = len & 0xff;
    ptr = 4;
  } else {
    buf[1] = 127;
    u_int64_t len = msg.payload_length;
    int i = 0;
    for (; i < 8; i++) {
      buf[2 + i] = len >> ((7 - i) * 8) & 0xff;
    }

    ptr = 2 + i;
  }

  memcpy(buf + ptr, msg.payload, msg.payload_length);

  frame->payload_length = msg.payload_length;
  frame->buf_length = (ptr) + msg.payload_length;
  frame->buf = buf;

  return 0;
}

int parse_ws_message(char *buf, struct ws_message *msg) {

  printf("First byte: %d, Second byte: %d\n", buf[0] & 0b10000000, buf[1]);
  if ((buf[0] & 0b10000000) != 0b10000000) {
    // INFO: For simplicity not handling FIN condition
    return -1;
  }

  char opcode = buf[0] & (char)0b1111;
  char mask = buf[1] & (char)0b10000000;

  printf("Opcode: %d, Mask: %d\n", opcode, mask != 0);

  size_t payload_length = buf[1] & (char)0b01111111;

  char mask_keys[4] = {0};
  int ptr = 0;

  if (payload_length <= 125) {
    ptr = 2;
  } else if (payload_length == 126) {
    payload_length = buf[2];
    payload_length = payload_length << 8;
    payload_length = payload_length | buf[3];
    ptr = 4;
  } else {
    payload_length = 0;
    for (int i = 0; i < 8; i++) {
      uint32_t num = buf[2 + i];
      payload_length = payload_length | num << ((7 - i) * 8);
    }

    ptr = 10;
  }

  for (int i = 0; i < 4; i++) {
    mask_keys[i] = buf[ptr + i];
  }

  ptr += 4;

  char *data_buf = malloc(payload_length + 1);

  for (int i = 0; i < payload_length; i++) {
    data_buf[i] = mask_keys[i % 4] ^ buf[ptr + i];
  }

  data_buf[payload_length] = 0; // for strings

  msg->opcode = opcode;
  msg->payload = data_buf;
  msg->payload_length = payload_length;

  return 0;
}
