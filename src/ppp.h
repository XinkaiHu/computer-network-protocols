#pragma once
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "checksum.h"
#include "util.h"

typedef enum PPP_PROTOCOL {
  PPP_PROTOCOL_IPD,
  PPP_PROTOCOL_NCD,
  PPP_PROTOCOL_LCD,
  PPP_PROTOCOL_AD
} PPP_PROTOCOL;

void send_ppp(data_t const input, data_t* output, PPP_PROTOCOL const protocol) {
  assert(input.size >= 0 && input.size <= 1500);
  output->size = input.size + 8;
  output->value = (uint8_t*)malloc(output->size);

  output->value[0] = 0X7E;
  output->value[1] = 0XFF;
  output->value[2] = 0X03;
  switch (protocol) {
    case PPP_PROTOCOL_IPD:
      *(uint16_t*)(output->value + 3) = 0X0021;
      break;
    case PPP_PROTOCOL_NCD:
      *(uint16_t*)(output->value + 3) = 0X8021;
      break;
    case PPP_PROTOCOL_LCD:
      *(uint16_t*)(output->value + 3) = 0XC021;
      break;
    case PPP_PROTOCOL_AD:
      *(uint16_t*)(output->value + 3) = 0XC023;
      break;
    default:
      assert(0);
  }
  memcpy(output->value + 5, input.value, input.size);
  *(uint16_t*)(output->value + (5 + input.size)) = crc16(*output, input.size + 5);
  output->value[7 + input.size] = 0X7E;
}

void parse_ppp(data_t const input, data_t* output, PPP_PROTOCOL* protocol) {
  output->size = input.size - 8;
  output->value = (uint8_t*)malloc(output->size);
  assert(output->size >= 0 && output->size <= 1500);

  switch (*(uint16_t*)(input.value + 3)) {
    case 0X0021:
      *protocol = PPP_PROTOCOL_IPD;
      break;
    case 0X8021:
      *protocol = PPP_PROTOCOL_NCD;
      break;
    case 0XC021:
      *protocol = PPP_PROTOCOL_LCD;
      break;
    case 0XC023:
      *protocol = PPP_PROTOCOL_AD;
      break;
    default:
      assert(0);
  }

  memcpy(output->value, input.value + 5, output->size);

  assert(*(uint16_t*)(input.value + (5 + output->size)) == crc16(input, output->size + 5));
}