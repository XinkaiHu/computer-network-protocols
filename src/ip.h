#pragma once
#include <stdio.h>
#include <string.h>

#include "checksum.h"
#include "util.h"

typedef enum IPv4_PROTOCOL_TYPE {
  IPv4_PROTOCOL_TYPE_ICMP,
  IPv4_PROTOCOL_TYPE_IGMP,
  IPv4_PROTOCOL_TYPE_IP,
  IPv4_PROTOCOL_TYPE_TCP,
  IPv4_PROTOCOL_TYPE_EGP,
  IPv4_PROTOCOL_TYPE_IGP,
  IPv4_PROTOCOL_TYPE_UDP,
  IPv4_PROTOCOL_TYPE_IPv6,
  IPv4_PROTOCOL_TYPE_ESP,
  IPv4_PROTOCOL_TYPE_AH,
  IPv4_PROTOCOL_TYPE_ICMP_IPv6,
  IPv4_PROTOCOL_TYPE_OSPF
} IPv4_PROTOCOL_TYPE;

void send_ip_v4(data_t const input, data_t* output, uint8_t const diff_serv,
                uint16_t const id, uint8_t const MF, uint8_t const DF,
                uint16_t const offset_byte, uint8_t const TTL,
                IPv4_PROTOCOL_TYPE const protocol, uint32_t const src_addr,
                uint32_t const dest_addr, data_t const optional) {
  assert(optional.size <= 40);
  assert((MF & 0XFE) == 0 && (DF & 0XFE) == 0);
  assert((offset_byte & 0X07) == 0);

  uint32_t filling_byte;
  filling_byte = (4 - (optional.size & 0X3)) % 4;
  uint16_t header_byte = 20 + optional.size + filling_byte;
  output->size = input.size + header_byte;
  assert(output->size <= 65535);
  output->value = (uint8_t*)malloc(output->size);
  output->value[0] = 4;
  output->value[0] |= (header_byte >> 2) << 4;
  output->value[1] = diff_serv;
  *(uint16_t*)(output->value + 2) = output->size;
  *(uint16_t*)(output->value + 4) = id;
  output->value[6] = MF;
  output->value[6] |= DF << 1;
  output->value[7] = 0;
  *(uint16_t*)(output->value + 6) |= offset_byte;
  output->value[8] = TTL;
  switch (protocol) {
    case IPv4_PROTOCOL_TYPE_ICMP:
      output->value[9] = 1;
      break;
    case IPv4_PROTOCOL_TYPE_IGMP:
      output->value[9] = 2;
      break;
    case IPv4_PROTOCOL_TYPE_IP:
      output->value[9] = 4;
      break;
    case IPv4_PROTOCOL_TYPE_TCP:
      output->value[9] = 6;
      break;
    case IPv4_PROTOCOL_TYPE_EGP:
      output->value[9] = 8;
      break;
    case IPv4_PROTOCOL_TYPE_IGP:
      output->value[9] = 9;
      break;
    case IPv4_PROTOCOL_TYPE_UDP:
      output->value[9] = 17;
      break;
    case IPv4_PROTOCOL_TYPE_IPv6:
      output->value[9] = 41;
      break;
    case IPv4_PROTOCOL_TYPE_ESP:
      output->value[9] = 50;
      break;
    case IPv4_PROTOCOL_TYPE_AH:
      output->value[9] = 51;
      break;
    case IPv4_PROTOCOL_TYPE_ICMP_IPv6:
      output->value[9] = 58;
      break;
    case IPv4_PROTOCOL_TYPE_OSPF:
      output->value[9] = 89;
      break;
    default:
      assert(0);
  }
  *(uint32_t*)(output->value + 12) = src_addr;
  *(uint32_t*)(output->value + 16) = dest_addr;
  memcpy(output->value + 20, optional.value, optional.size);
  memset(output->value + (20 + optional.size), 0, filling_byte);

  *(uint16_t*)(output->value + 10) = 0X0000;
  *(uint16_t*)(output->value + 10) = checksum_ip(*output, header_byte);
  memcpy(output->value + header_byte, input.value, input.size);
}

void parse_ip_v4(data_t const input, data_t* output, uint8_t* version,
                 uint8_t* header_byte, uint8_t* diff_serv, uint16_t* total_byte,
                 uint16_t* id, uint8_t* MF, uint8_t* DF, uint16_t* offset_byte,
                 uint8_t* TTL, IPv4_PROTOCOL_TYPE* protocol, uint32_t* src_addr,
                 uint32_t* dest_addr, data_t* optional) {
  assert((input.value[0] & 0X0F) == 4);
  *version = 4;
  *header_byte = (input.value[0] >> 4) << 2;
  *diff_serv = input.value[1];
  *total_byte = *(uint16_t*)(input.value + 2);
  *id = *(uint16_t*)(input.value + 4);
  *MF = input.value[6] & 1;
  *DF = (input.value[6] >> 1) & 1;
  *offset_byte = *(uint16_t*)(input.value + 6) & 0XF8;
  *TTL = input.value[8];
  switch (input.value[9]) {
    case 1:
      *protocol = IPv4_PROTOCOL_TYPE_ICMP;
      break;
    case 2:
      *protocol = IPv4_PROTOCOL_TYPE_IGMP;
      break;
    case 4:
      *protocol = IPv4_PROTOCOL_TYPE_IP;
      break;
    case 6:
      *protocol = IPv4_PROTOCOL_TYPE_TCP;
      break;
    case 8:
      *protocol = IPv4_PROTOCOL_TYPE_EGP;
      break;
    case 9:
      *protocol = IPv4_PROTOCOL_TYPE_IGP;
      break;
    case 17:
      *protocol = IPv4_PROTOCOL_TYPE_UDP;
      break;
    case 41:
      *protocol = IPv4_PROTOCOL_TYPE_IPv6;
      break;
    case 50:
      *protocol = IPv4_PROTOCOL_TYPE_ESP;
      break;
    case 51:
      *protocol = IPv4_PROTOCOL_TYPE_AH;
      break;
    case 58:
      *protocol = IPv4_PROTOCOL_TYPE_ICMP_IPv6;
      break;
    case 89:
      *protocol = IPv4_PROTOCOL_TYPE_OSPF;
      break;
    default:
      assert(0);
  }
  assert(0 == checksum_ip(input, *header_byte));
  *src_addr = *(uint32_t*)(input.value + 12);
  *dest_addr = *(uint32_t*)(input.value + 16);
  optional->size = *header_byte - 20;
  if (optional->size > 0) {
    optional->value = (uint8_t*)malloc(optional->size);
    memcpy(optional->value, input.value + 20, optional->size);
  }
  output->size = *total_byte - *header_byte;
  output->value = (uint8_t*)malloc(output->size);
  memcpy(output->value, input.value + *header_byte, output->size);
}