#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// char *sha1(char *str) {
//   size_t size = strlen(str);
//   printf("size: %ld\n", size);
//   size_t str_size = size;
//
//   size++;
//
//   printf("size: %ld\n", size);
//   size = size + (64 - (size % 64));
//   printf("size: %ld\n", size);
//
//   char *buffer = malloc(size);
//   memset(buffer, 0, size);
//   memcpy(buffer, str, str_size);
//
//   buffer[str_size] = ((char)1) << 7;
//
//   uint64_t int_str_size = (uint64_t)str_size;
//
//   size_t offset = (((size / 64) - 1) * 64) + 56;
//
//   uint64_t mask = ((uint64_t)0b11111111 << (7 * 8));
//   // uint64_t mask = 0b11111111;
//   printf("mask: %lb,offset: %ld, size: %lb\n", mask, offset, int_str_size);
//
//   for (int i = 0; i < 8; i++) {
//     buffer[offset + i] = (int_str_size & (mask >> (i * 8))) >> ((7 - i) * 8);
//     // buffer[offset + i] = int_str_size & (mask << (i * 8))>> ((7-i)*8);
//   }
//
//   for (int i = 0; i < size; i++) {
//     printf("%b\n", (char)buffer[i]);
//   }
//
//   const uint32_t H[] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476,
//                         0xC3D2E1F0};
//
//   // INFO: Hope the type cast helps
//   uint32_t *new_buf = (uint32_t *)buffer;
//   for (int i = 0; size / 64; i++) {
//     for (int t = 0; t < 80; t++) {
//     }
//   }
//
//   return buffer;
// }

int main(int argc, char *argv[]) {

  FILE *fp;
  char buffer[41];

  fp = popen("echo -n hello | sha1sum", "r");
  if (fp == NULL) {
    perror("popen failed");
    return 1;
  }

  fgets(buffer, 41, fp);
  // printf("Got: %s", buffer);
  // Close the pipe and get exit status
  int status = pclose(fp);
  printf("\nChild finished with status %d\n", status);
  fclose(fp);

  return 0;
}
