#include "utils.h"

void lower_case(char *str) {
  for (int i = 0; str[i]; i++) {
    char ch = str[i];

    if (ch >= 'A' && ch <= 'Z') {
      str[i] += 32;
    }
  }
}
