#pragma once
#include <string.h>

#include "util.h"

void byte_stuffing(data_t const input, data_t* output) {
  assert(input.value[0] == 0X7E && input.value[input.size - 1] == 0X7E);
  uint32_t input_pos;
  uint32_t output_pos;

  output->size = input.size;
  for (int i = 1; i < input.size - 1; ++i) {
    if (input.value[i] == 0X7E || input.value[i] == 0X7D ||
        input.value[i] < 0X20) {
      ++output->size;
    }
  }

  output->value = (uint8_t*)malloc(output->size);

  output->value[0] = input.value[0];
  output->value[output->size - 1] = input.value[input.size - 1];
  for (output_pos = 1, input_pos = 1; input_pos < input.size - 1; ++input_pos) {
    if (input.value[input_pos] == 0X7E || input.value[input_pos] == 0X7D) {
      output->value[output_pos++] = 0X7D;
      output->value[output_pos++] = input.value[input_pos] - 0X20;
    } else if (input.value[input_pos] < 0X20) {
      output->value[output_pos++] = 0X7D;
      output->value[output_pos++] = input.value[input_pos] + 0X20;
    } else {
      output->value[output_pos++] = input.value[input_pos];
    }
  }
}

void parse_byte_stuffing(data_t const input, data_t* output) {
  assert(input.value[0] == 0X7E && input.value[input.size - 1] == 0X7E);
  uint32_t input_pos;
  uint32_t output_pos;

  output->size = input.size;
  for (int i = 1; i < input.size - 1; ++i) {
    if (input.value[i] == 0X7D) {
      --output->size;
    }
  }

  output->value = (uint8_t*)malloc(output->size);

  output->value[0] = input.value[0];
  output->value[output->size - 1] = input.value[input.size - 1];
  for (output_pos = 1, input_pos = 1; input_pos < input.size - 1; ++input_pos) {
    if (input.value[input_pos] == 0X7D) {
      if (input.value[input_pos + 1] == 0X5E ||
          input.value[input_pos + 1] == 0X5D) {
        output->value[output_pos++] = input.value[input_pos + 1] + 0X20;
        ++input_pos;
      } else if (input.value[input_pos + 1] < 0X40) {
        output->value[output_pos++] = input.value[input_pos + 1] - 0X20;
        ++input_pos;
      } else {
        assert(0);
      }
    } else {
      output->value[output_pos++] = input.value[input_pos];
    }
  }
}

void bit_stuffing(data_t const input, data_t* output) {
  assert(input.value[0] == 0X7E && input.value[input.size - 1] == 0X7E);

  data_t data_bit;
  uint32_t one_count;
  uint32_t input_pos;
  uint32_t output_pos;

  char_to_binary(input, &data_bit);
  output->size = data_bit.size;
  one_count = 0;

  for (int i = 8; i < data_bit.size - 8; ++i) {
    if (data_bit.value[i] == '1') {
      ++one_count;
    } else {
      one_count = 0;
    }

    if (one_count == 5) {
      ++output->size;
      one_count = 0;
    }
  }

  output->value = (uint8_t*)malloc(output->size);
  memcpy(output->value, data_bit.value, 8);
  memcpy(output->value + (output->size - 8),
         data_bit.value + (data_bit.size - 8), 8);
  one_count = 0;

  for (input_pos = 8, output_pos = 8; input_pos < data_bit.size - 8;
       ++input_pos) {
    output->value[output_pos++] = data_bit.value[input_pos];
    if (data_bit.value[input_pos] == '1') {
      ++one_count;
    } else {
      one_count = 0;
    }
    if (one_count == 5) {
      output->value[output_pos++] = '0';
    }
  }

  free(data_bit.value);
}

void parse_bit_stuffing(data_t const input, data_t* output) {
  data_t data_bit;
  uint32_t one_count;
  uint32_t input_pos;
  uint32_t output_pos;

  data_bit.size = input.size;
  one_count = 0;

  for (int i = 8; i < input.size - 8; ++i) {
    if (input.value[i] == '1') {
      ++one_count;
    } else {
      one_count = 0;
    }
    assert(one_count < 6);
    if (one_count == 5) {
      --data_bit.size;
    }
  }

  data_bit.value = (uint8_t*)malloc(data_bit.size);
  memcpy(data_bit.value, input.value, 8);
  memcpy(data_bit.value + (data_bit.size - 8), input.value + (input.size - 8),
         8);
  one_count = 0;

  for (input_pos = 8, output_pos = 8; input_pos < input.size - 8; ++input_pos) {
    data_bit.value[output_pos++] = input.value[input_pos];
    if (input.value[input_pos] == '1') {
      ++one_count;
    } else {
      one_count = 0;
    }
    if (one_count == 5) {
      ++input_pos;
    }
  }

  binary_to_char(data_bit, output);

  free(data_bit.value);
}