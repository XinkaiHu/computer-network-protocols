#include <stdio.h>

#include "../src/ip.h"
#include "../src/util.h"

int main(void) {
  data_t original_data = {
      .value = "a simple test message for framing and parsing of IPv4 protocol",
      .size = 63};
  uint8_t diff_serv = 1;
  uint16_t id = 1;
  uint8_t MF = 1;
  uint8_t DF = 0;
  uint16_t offset = 24;
  uint8_t TTL = 4;
  IPv4_PROTOCOL_TYPE protocol = IPv4_PROTOCOL_TYPE_OSPF;
  uint32_t src_addr = 0X12345678;
  uint32_t dest_addr = 0X87654321;
  data_t optional = {.value = "A", .size = 2};

  data_t ip_datagram_hex;
  data_t ip_datagram;

  data_t parsed_data;
  uint8_t parsed_version;
  uint8_t parsed_header_byte;
  uint8_t parsed_diff_serv;
  uint16_t parsed_total_byte;
  uint16_t parsed_id;
  uint8_t parsed_MF;
  uint8_t parsed_DF;
  uint16_t parsed_offset_byte;
  uint8_t parsed_TTL;
  IPv4_PROTOCOL_TYPE parsed_protocol;
  uint32_t parsed_src_addr;
  uint32_t parsed_dest_addr;
  data_t parsed_optional;

  send_ip_v4(original_data, &ip_datagram, diff_serv, id, MF, DF, offset, TTL,
             protocol, src_addr, dest_addr, optional);
  parse_ip_v4(ip_datagram, &parsed_data, &parsed_version, &parsed_header_byte,
              &parsed_diff_serv, &parsed_total_byte, &parsed_id, &parsed_MF,
              &parsed_DF, &parsed_offset_byte, &parsed_TTL, &parsed_protocol,
              &parsed_src_addr, &parsed_dest_addr, &parsed_optional);

  char_to_hex(ip_datagram, &ip_datagram_hex);

  printf("IPv4 datagram: %s\nSize: %d\n\n", ip_datagram_hex.value,
         ip_datagram.size);
  printf(
      "Parsed data: %s\nHeader byte: %d\nDiffServ: %d\nTotal byte: %d\nID: "
      "%d\nMF: %d\nDF: %d\nOffset byte: %d\nTTL: %d\nProtocol: %d\nSource "
      "address: %x\nDestination address: %x\nOptional: %s\n\n",
      parsed_data.value, parsed_header_byte, parsed_diff_serv,
      parsed_total_byte, parsed_id, parsed_MF, parsed_DF, parsed_offset_byte,
      parsed_TTL, parsed_protocol, parsed_src_addr, parsed_dest_addr,
      parsed_optional.value);
  free(ip_datagram.value);
  free(ip_datagram_hex.value);
  free(parsed_data.value);
  free(parsed_optional.value);
}