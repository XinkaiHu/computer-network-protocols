#include "checksum.h"
#include "util.h"

void send_tcp(data_t const input, data_t* output, uint16_t const src_port,
              uint16_t const dest_port, uint32_t const id,
              uint32_t const ack_value, uint8_t const URG, uint8_t const ACK,
              uint8_t const PSH, uint8_t const RST, uint8_t const SYN,
              uint8_t const FIN, uint16_t const window, uint16_t const urg_ptr,
              data_t const optional, uint32_t const src_addr,
              uint32_t const dest_addr) {
  assert(optional.size <= 40);
  assert((URG & 0XFE) == 0 && (ACK & 0XFE) == 0 && (PSH & 0XFE) == 0 &&
         (RST & 0XFE) == 0 && (SYN & 0XFE) == 0 && (FIN & 0XFE) == 0);
  assert(optional.size <= 40);

  data_t tmp_header;

  uint32_t filling_byte;
  filling_byte = (4 - (optional.size & 0X3)) % 4;
  uint8_t header_byte = 20 + optional.size + filling_byte;
  output->size = header_byte + input.size;
  output->value = (uint8_t*)malloc(output->size);

  *(uint16_t*)output->value = src_port;
  *(uint16_t*)(output->value + 2) = dest_port;
  *(uint32_t*)(output->value + 4) = id;
  *(uint32_t*)(output->value + 8) = ack_value;
  output->value[12] = header_byte >> 2;
  output->value[13] = 0;
  output->value[13] |= URG << 2;
  output->value[13] |= ACK << 3;
  output->value[13] |= PSH << 4;
  output->value[13] |= RST << 5;
  output->value[13] |= SYN << 6;
  output->value[13] |= FIN << 7;
  *(uint16_t*)(output->value + 14) = window;
  *(uint16_t*)(output->value + 16) = 0;
  *(uint16_t*)(output->value + 18) = urg_ptr;
  memcpy(output->value + 20, optional.value, optional.size);
  memset(output->value + 20 + optional.size, 0, filling_byte);
  memcpy(output->value + header_byte, input.value, input.size);

  tmp_header.size = 12;
  tmp_header.value = (uint8_t*)malloc(tmp_header.size);

  *(uint32_t*)tmp_header.value = src_addr;
  *(uint32_t*)(tmp_header.value + 4) = dest_addr;
  tmp_header.value[8] = 0;
  tmp_header.value[9] = 6;
  *(uint16_t*)(tmp_header.value + 10) = output->size;
  *(uint16_t*)(output->value + 16) = checksum(*output, tmp_header);

  free(tmp_header.value);
}

void parse_tcp(data_t const input, data_t* output, uint16_t* src_port,
               uint16_t* dest_port, uint32_t* id, uint32_t* ack_value,
               uint8_t* offset_byte, uint8_t* URG, uint8_t* ACK, uint8_t* PSH,
               uint8_t* RST, uint8_t* SYN, uint8_t* FIN, uint16_t* window,
               uint16_t* urg_ptr, data_t* optional, uint32_t const src_addr,
               uint32_t const dest_addr) {
  data_t tmp_header;
  tmp_header.size = 12;
  tmp_header.value = (uint8_t*)malloc(tmp_header.size);

  *(uint32_t*)tmp_header.value = src_addr;
  *(uint32_t*)(tmp_header.value + 4) = dest_addr;
  tmp_header.value[8] = 0;
  tmp_header.value[9] = 6;
  *(uint16_t*)(tmp_header.value + 10) = input.size;
  assert(0 == checksum(input, tmp_header));

  *offset_byte = input.value[12] << 2;
  assert(*offset_byte <= 60);
  *src_port = *(uint16_t*)input.value;
  *dest_port = *(uint16_t*)(input.value + 2);
  *id = *(uint32_t*)(input.value + 4);
  *ack_value = *(uint32_t*)(input.value + 8);
  *URG = (input.value[13] >> 2) & 1;
  *ACK = (input.value[13] >> 3) & 1;
  *PSH = (input.value[13] >> 4) & 1;
  *RST = (input.value[13] >> 5) & 1;
  *SYN = (input.value[13] >> 6) & 1;
  *FIN = (input.value[13] >> 7) & 1;
  *window = *(uint16_t*)(input.value + 14);
  *urg_ptr = *(uint16_t*)(input.value + 18);
  optional->size = *offset_byte - 20;
  if (optional->size > 0) {
    optional->value = (uint8_t*)malloc(optional->size);
    memcpy(optional->value, input.value + 20, optional->size);
  }
  output->size = input.size - *offset_byte;
  output->value = (uint8_t*)malloc(output->size);
  memcpy(output->value, input.value + *offset_byte, output->size);

  free(tmp_header.value);
}