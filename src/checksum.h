#pragma once
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

uint16_t crc16(data_t const input) {
  uint8_t i;
  uint16_t crc_value;
  uint16_t size;
  uint8_t* value;

  size = input.size;
  value = input.value;
  crc_value = 0XFFFF;
  while (size--) {
    crc_value ^= *value++;
    for (i = 0; i < 8; ++i) {
      if (crc_value & 0X0001) {
        crc_value = (crc_value >> 1) ^ 0XA001;
      } else {
        crc_value >>= 1;
      }
    }
  }

  crc_value = ~crc_value;
  return crc_value;
}

uint32_t crc32(data_t const input) {
  uint8_t i;
  uint32_t crc_value;
  uint16_t size;
  uint8_t* value;

  size = input.size;
  value = input.value;

  crc_value = 0XFFFFFFFF;
  while (size--) {
    crc_value ^= *value++;
    for (i = 0; i < 8; ++i) {
      if (crc_value & 0X0001) {
        crc_value = (crc_value >> 1) ^ 0XEDB88320;
      } else {
        crc_value >>= 1;
      }
    }
  }

  crc_value = ~crc_value;
  return crc_value;
}

uint16_t checksum(data_t const input, data_t const tmp_header) {
  uint32_t checksum_help = 0;
  for (uint8_t i = 0; i < input.size - 1; i += 2) {
    checksum_help += *(uint16_t*)(input.value + i);
  }
  if (input.size & 1) {
    checksum_help += (uint16_t)(input.value[input.size - 1]) << 8;
  } else {
    checksum_help += *(uint16_t*)(input.value + (input.size - 2));
  }
  for (uint8_t i = 0; i < tmp_header.size; i += 2) {
    checksum_help += *(uint16_t*)(tmp_header.value + i);
  }
  while (checksum_help > 0XFFFF) {
    checksum_help = (checksum_help >> 16) + (uint16_t)checksum_help;
  }
  uint16_t checksum;
  checksum = (uint16_t)checksum_help;
  checksum = ~checksum;
  return checksum;
}

uint16_t checksum_ip(data_t const input, uint16_t n) {
  uint32_t checksum_help = 0;
  for (uint8_t i = 0; i < n; i += 2) {
    checksum_help += *(uint16_t*)(input.value + i);
  }
  while (checksum_help > 0XFFFF) {
    checksum_help = (checksum_help >> 16) + (uint16_t)checksum_help;
  }
  uint16_t checksum;
  checksum = (uint16_t)checksum_help;
  checksum = ~checksum;
  return checksum;
}