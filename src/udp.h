#pragma once
#include "checksum.h"
#include "util.h"

void send_udp(data_t const input, data_t* output, uint16_t const src_port,
              uint16_t const dest_port, uint32_t const src_addr,
              uint32_t const dest_addr) {
  data_t tmp_header;
  tmp_header.size = 12;
  tmp_header.value = (uint8_t*)malloc(tmp_header.size);

  output->size = input.size + 8;
  output->value = (uint8_t*)malloc(output->size);
  *(uint16_t*)output->value = src_port;
  *(uint16_t*)(output->value + 2) = dest_port;
  *(uint16_t*)(output->value + 4) = 8 + input.size;
  memcpy(output->value + 8, input.value, input.size);

  *(uint32_t*)tmp_header.value = src_addr;
  *(uint32_t*)(tmp_header.value + 4) = dest_addr;
  tmp_header.value[8] = 0X00;
  tmp_header.value[9] = 0X11;
  *(uint16_t*)(tmp_header.value + 10) = output->size;
  *(uint16_t*)(output->value + 6) = checksum(*output, tmp_header);

  free(tmp_header.value);
}

void parse_udp(data_t const input, data_t* output, uint16_t* src_port,
               uint16_t* dest_port, uint32_t const src_addr,
               uint32_t const dest_addr) {
  assert(input.size == *(uint16_t*)(input.value + 4));
  data_t tmp_header;
  tmp_header.size = 12;
  tmp_header.value = (uint8_t*)malloc(tmp_header.size);

  *(uint32_t*)tmp_header.value = src_addr;
  *(uint32_t*)(tmp_header.value + 4) = dest_addr;
  tmp_header.value[8] = 0X00;
  tmp_header.value[9] = 0X11;
  *(uint16_t*)(tmp_header.value + 10) = input.size;

  assert(0 == checksum(input, tmp_header));

  output->size = input.size - 8;
  output->value = (uint8_t*)malloc(output->size);
  memcpy(output->value, input.value + 8, output->size);

  *src_port = *(uint16_t*)input.value;
  *dest_port = *(uint16_t*)(input.value + 2);

  free(tmp_header.value);
}