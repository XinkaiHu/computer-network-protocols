#include <stdio.h>
#include <stdlib.h>

#include "../src/udp.h"
#include "../src/util.h"

int main(void) {
  data_t original_data = {
      .value = "a simple test message for framing and parsing of UDP datagram",
      .size = 62};
  uint32_t src_addr = 0X12345678;
  uint32_t dest_addr = 0X87654321;
  uint16_t src_port = 0X4919;
  uint16_t dest_port = 0X1234;

  data_t udp_datagram;
  data_t udp_datagram_hex;

  data_t parsed_data;
  uint16_t parsed_src_port;
  uint16_t parsed_dest_port;

  send_udp(original_data, &udp_datagram, src_port, dest_port, src_addr,
           dest_addr);
  parse_udp(udp_datagram, &parsed_data, &parsed_src_port, &parsed_dest_port,
            src_addr, dest_addr);

  char_to_hex(udp_datagram, &udp_datagram_hex);

  printf("UDP datagram: %s\nSize: %d\n\n", udp_datagram_hex.value,
         udp_datagram.size);
  printf("Parsed data: %s\nSize: %d\nSource port: %x\nDestination port: %x\n\n",
         parsed_data.value, parsed_data.size, parsed_src_port,
         parsed_dest_port);

  free(udp_datagram.value);
  free(udp_datagram_hex.value);
  free(parsed_data.value);
}