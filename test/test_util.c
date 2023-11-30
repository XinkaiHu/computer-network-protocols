#include <stdio.h>
#include <stdlib.h>

#include "../src/util.h"

int main(void) {
  uint8_t value[] = {0, 1,  2,  3,  4,  5,  6,  7, 8,
                           9, 10, 11, 12, 13, 14, 15, 16};
  data_t data = {.value = value, .size = 17};
  data_t hex;
  data_t binary;

  char_to_binary(data, &binary);
  char_to_hex(data, &hex);

  printf("hex: %s\nSize: %d\n\n", hex.value, hex.size);
  printf("binary: %s\nSize: %d\n\n", binary.value, binary.size);

  free(hex.value);
  free(binary.value);
}