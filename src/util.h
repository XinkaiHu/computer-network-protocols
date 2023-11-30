#pragma once
#include <assert.h>
#include <stdlib.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

typedef struct data_t {
  uint8_t* value;
  uint32_t size;
} data_t;

void char_to_binary(data_t const input, data_t* output) {
  output->size = input.size * 8;
  output->value = (uint8_t*)malloc(output->size + 1);

  for (int i = 0; i < input.size; ++i) {
    for (int j = 0; j < 8; ++j) {
      output->value[i * 8 + j] = ((input.value[i] >> (7 - j)) & 1) ? '1' : '0';
    }
  }
  output->value[output->size] = '\0';
}

void binary_to_char(data_t const input, data_t* output) {
  assert((input.size & 0X7) == 0);
  output->size = (input.size - 1) / 8 + 1;
  output->value = (uint8_t*)malloc(output->size);

  for (int i = 0; i < output->size; ++i) {
    output->value[i] = 0;
    for (int j = 0; j < 8; ++j) {
      output->value[i] |= (input.value[i * 8 + j] == '1' ? 1 : 0) << (7 - j);
    }
  }
}

void char_to_hex(data_t const input, data_t* output) {
  uint8_t const hex_table[] = "0123456789ABCDEF";
  output->size = input.size * 2;
  output->value = (uint8_t*)malloc(output->size + 1);

  for (int i = 0; i < input.size; ++i) {
    output->value[i << 1] = hex_table[(input.value[i] >> 4) & 0xF];
    output->value[1 + (i << 1)] = hex_table[input.value[i] & 0XF];
  }
  output->value[output->size] = '\0';
}