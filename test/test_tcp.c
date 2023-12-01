#include <stdio.h>
#include <stdlib.h>

#include "../src/tcp.h"
#include "../src/util.h"

int main(void) {
  data_t original_data = {
      .value = "a simple test message for framing and parsing of TCP datagram",
      .size = 62};
  uint32_t src_addr = 0X12345678;
  uint32_t dest_addr = 0X87654321;
  uint16_t src_port = 0X4919;
  uint16_t dest_port = 0X1234;
  uint32_t id = 0X23333333;
  uint32_t ack_value = 0X20000000;
  uint8_t URG = 0;
  uint8_t ACK = 1;
  uint8_t PSH = 0;
  uint8_t RST = 1;
  uint8_t SYN = 0;
  uint8_t FIN = 1;
  uint16_t window = 2048;
  uint16_t urg_ptr = 0;
  data_t optional = {.value = "some optional data", .size = 19};

  data_t tcp_datagram;
  data_t tcp_datagram_hex;

  data_t parsed_data;
  uint16_t parsed_src_port;
  uint16_t parsed_dest_port;
  uint32_t parsed_id;
  uint32_t parsed_ack_value;
  uint8_t parsed_offset_byte;
  uint8_t parsed_URG;
  uint8_t parsed_ACK;
  uint8_t parsed_PSH;
  uint8_t parsed_RST;
  uint8_t parsed_SYN;
  uint8_t parsed_FIN;
  uint16_t parsed_window;
  uint16_t parsed_urg_ptr;
  data_t parsed_optional;

  send_tcp(original_data, &tcp_datagram, src_port, dest_port, id, ack_value,
           URG, ACK, PSH, RST, SYN, FIN, window, urg_ptr, optional, src_addr,
           dest_addr);
  parse_tcp(tcp_datagram, &parsed_data, &parsed_src_port, &parsed_dest_port,
            &parsed_id, &parsed_ack_value, &parsed_offset_byte, &parsed_URG,
            &parsed_ACK, &parsed_PSH, &parsed_RST, &parsed_SYN, &parsed_FIN,
            &parsed_window, &parsed_urg_ptr, &parsed_optional, src_addr,
            dest_addr);

  char_to_hex(tcp_datagram, &tcp_datagram_hex);

  printf("TCP datagram: %s\nSize: %d\n\n", tcp_datagram_hex.value,
         tcp_datagram.size);
  printf(
      "Parsed data: %s\nSize: %d\nSource port: %x\nDestination port: %x\nID: "
      "%x\nACK value: %x\nHeader byte: %d\nURG: %d\nACK: %d\nPSH: %d\nRST: "
      "%d\nSYN: %d\nFIN: %d\nWindow: %d\nURG pointer: %x\nOptional data: "
      "%s\n\n",
      parsed_data.value, parsed_data.size, parsed_src_port, parsed_dest_port,
      parsed_id, parsed_ack_value, parsed_offset_byte, parsed_URG, parsed_ACK,
      parsed_PSH, parsed_RST, parsed_SYN, parsed_FIN, parsed_window,
      parsed_urg_ptr, parsed_optional.value);

  free(tcp_datagram.value);
  free(tcp_datagram_hex.value);
  free(parsed_data.value);
  if (parsed_optional.size > 0) {
    free(parsed_optional.value);
  }
}