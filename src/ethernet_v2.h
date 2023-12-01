#pragma once
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "checksum.h"
#include "util.h"

typedef enum ETHERNET_V2_TYPE {
  ETHERNET_V2_TYPE_IPv4,
  ETHERNET_V2_TYPE_ARP,
  ETHERNET_V2_TYPE_PPPoE,
  ETHERNET_V2_TYPE_802_1Q,
  ETHERNET_V2_TYPE_IPv6
} ETHERNET_V2_TYPE;

void send_ethernet_v2(data_t const input, data_t* output,
                      data_t const dest_addr, data_t const src_addr,
                      ETHERNET_V2_TYPE const type) {
  assert(input.size <= 1500);
  assert(src_addr.size == 6 && dest_addr.size == 6);
  output->size = input.size < 46 ? 46 : (input.size + 18);
  output->value = (uint8_t*)malloc(output->size);

  memcpy(output->value, dest_addr.value, 6);
  memcpy(output->value + 6, src_addr.value, 6);
  switch (type) {
    case ETHERNET_V2_TYPE_IPv4:
      *(uint16_t*)(output->value + 12) = 0X0800;
      break;
    case ETHERNET_V2_TYPE_ARP:
      *(uint16_t*)(output->value + 12) = 0X0806;
      break;
    case ETHERNET_V2_TYPE_PPPoE:
      *(uint16_t*)(output->value + 12) = 0X8864;
      break;
    case ETHERNET_V2_TYPE_802_1Q:
      *(uint16_t*)(output->value + 12) = 0X8100;
      break;
    case ETHERNET_V2_TYPE_IPv6:
      *(uint16_t*)(output->value + 12) = 0X86DD;
      break;
    default:
      assert(0);
  }
  memcpy(output->value + 14, input.value, input.size);
  if (input.size < 46) {
    memset(output->value + (14 + input.size), 0, 46 - input.size);
  }
  *(uint32_t*)(output->value + (14 + input.size)) =
      crc32(*output, 14 + input.size);
}

void parse_ethernet_v2(data_t const input, data_t* output, data_t* dest_addr,
                       data_t* src_addr, ETHERNET_V2_TYPE* type) {
  assert(input.size >= 64);

  output->size = input.size - 18;
  output->value = (uint8_t*)malloc(output->size);
  dest_addr->size = 6;
  dest_addr->value = (uint8_t*)malloc(dest_addr->size);
  src_addr->size = 6;
  src_addr->value = (uint8_t*)malloc(src_addr->size);
  memcpy(dest_addr->value, input.value, 6);
  memcpy(src_addr->value, input.value + 6, 6);
  switch (*(uint16_t*)(input.value + 12)) {
    case 0X0800:
      *type = ETHERNET_V2_TYPE_IPv4;
      break;
    case 0X0806:
      *type = ETHERNET_V2_TYPE_ARP;
      break;
    case 0X8864:
      *type = ETHERNET_V2_TYPE_PPPoE;
      break;
    case 0X8100:
      *type = ETHERNET_V2_TYPE_802_1Q;
      break;
    case 0X86DD:
      *type = ETHERNET_V2_TYPE_IPv6;
      break;
    default:
      assert(0);
  }
  memcpy(output->value, input.value + 14, output->size);

  assert(*(uint32_t*)(input.value + (14 + output->size)) ==
         crc32(input, 14 + output->size));
}