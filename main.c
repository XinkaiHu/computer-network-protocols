#include <stdio.h>

#include "src/ethernet_v2.h"
#include "src/ip.h"
#include "src/ppp.h"
#include "src/stuffing.h"
#include "src/tcp.h"
#include "src/udp.h"
#include "src/util.h"

int main(void) {
  data_t original_data = {
      .value =
          "a simple test message which is \aprocessed into \nUDP datagram, "
          "\nIPv4 datagram, \tEthernet v2.\nAfter all the works, \bit will be "
          "parsed into its original shape.\nHope a good luck.\n\n",
      .size = 177};

  uint16_t src_port = 0X1234;
  uint16_t dest_port = 0X4919;
  uint32_t src_ip_addr = 0X12345678;
  uint32_t dest_ip_addr = 0X87654321;

  data_t src_mac_addr = {.value = "012345", .size = 6};
  data_t dest_mac_addr = {.value = "FEDCBA", .size = 6};
  ETHERNET_V2_TYPE type = ETHERNET_V2_TYPE_PPPoE;

  uint8_t diff_serv = 1;
  uint16_t ip_id = 1;
  uint8_t MF = 1;
  uint8_t DF = 0;
  uint16_t ip_offset_byte = 24;
  uint8_t TTL = 4;
  IPv4_PROTOCOL_TYPE ip_protocol = IPv4_PROTOCOL_TYPE_UDP;

  uint32_t tcp_id = 0X23333333;
  uint32_t ack_value = 0X20000000;
  uint8_t URG = 0;
  uint8_t ACK = 1;
  uint8_t PSH = 0;
  uint8_t RST = 0;
  uint8_t SYN = 0;
  uint8_t FIN = 1;
  uint16_t window = 64;
  uint16_t urg_ptr = 0;
  data_t optional = {.value = "Optional configurations.", .size = 25};

  PPP_PROTOCOL ppp_protocol = PPP_PROTOCOL_IPD;

  data_t tcp_datagram;
  data_t udp_datagram;
  data_t ip_datagram;
  data_t ppp_frame;
  data_t mac_frame;
  data_t bit_stuffing_stream;

  data_t parsed_bit_stuffing_stream;
  data_t parsed_mac_frame;
  data_t parsed_ppp_frame;
  data_t parsed_ip_datagram;
  data_t parsed_udp_datagram;
  data_t parsed_tcp_datagram;

  uint32_t parsed_tcp_id;
  uint32_t parsed_ack_value;
  uint8_t parsed_tcp_offset_byte;
  uint8_t parsed_URG;
  uint8_t parsed_ACK;
  uint8_t parsed_PSH;
  uint8_t parsed_RST;
  uint8_t parsed_SYN;
  uint8_t parsed_FIN;
  uint16_t parsed_window;
  uint16_t parsed_urg_ptr;

  uint16_t parsed_src_port;
  uint16_t parsed_dest_port;
  uint32_t parsed_src_ip_addr;
  uint32_t parsed_dest_ip_addr;

  uint8_t parsed_version;
  uint8_t parsed_header_byte;
  uint8_t parsed_diff_serv;
  uint16_t parsed_total_byte;
  uint16_t parsed_ip_id;
  uint8_t parsed_MF;
  uint8_t parsed_DF;
  uint16_t parsed_ip_offset_byte;
  uint8_t parsed_TTL;
  IPv4_PROTOCOL_TYPE parsed_protocol;
  data_t parsed_optional;

  PPP_PROTOCOL parsed_ppp_protocol;

  ETHERNET_V2_TYPE parsed_type;
  data_t parsed_src_mac_addr;
  data_t parsed_dest_mac_addr;

  send_udp(original_data, &udp_datagram, src_port, dest_port, src_ip_addr,
           dest_ip_addr);
  send_ip_v4(udp_datagram, &ip_datagram, diff_serv, ip_id, MF, DF,
             ip_offset_byte, TTL, ip_protocol, src_ip_addr, dest_ip_addr,
             optional);
  send_ppp(ip_datagram, &ppp_frame, ppp_protocol);
  send_ethernet_v2(ppp_frame, &mac_frame, dest_mac_addr, src_mac_addr, type);
  parse_ethernet_v2(mac_frame, &parsed_mac_frame, &parsed_dest_mac_addr,
                    &parsed_src_mac_addr, &parsed_type);
  parse_ppp(parsed_mac_frame, &parsed_ppp_frame, &parsed_ppp_protocol);
  parse_ip_v4(parsed_ppp_frame, &parsed_ip_datagram, &parsed_version,
              &parsed_header_byte, &parsed_diff_serv, &parsed_total_byte,
              &parsed_ip_id, &parsed_MF, &parsed_DF, &parsed_ip_offset_byte,
              &parsed_TTL, &parsed_protocol, &parsed_src_ip_addr,
              &parsed_dest_ip_addr, &parsed_optional);
  parse_udp(parsed_ip_datagram, &parsed_udp_datagram, &parsed_src_port,
            &parsed_dest_port, parsed_src_ip_addr, parsed_dest_ip_addr);

  printf(
      "Parsed data: %s\nSize: %d\n\nEthernet v2...\nSource Mac address: "
      "%s\nDestination Mac address: %s\nType: %d\n\nPPP...\nProtocol: "
      "%d\n\nIP...\nVersion: %d\nHeader byte: %d\nDiffServ: %d\nTotal byte: "
      "%d\nID: %d\nMF: %d\nDF: %d\nOffset byte: %d\nTTL: %d\nProtocol: "
      "%d\nSource IP address: %x\nDestination IP address: %x\nOptional data: "
      "%s\n\nUDP...\nSource port: %x\nDestination port: %x\n\n\n",
      parsed_udp_datagram.value, parsed_udp_datagram.size,
      parsed_src_mac_addr.value, parsed_dest_mac_addr.value, parsed_type,
      parsed_ppp_protocol, parsed_version, parsed_header_byte, parsed_diff_serv,
      parsed_total_byte, parsed_tcp_id, parsed_MF, parsed_DF,
      parsed_tcp_offset_byte, parsed_TTL, parsed_protocol, parsed_src_ip_addr,
      parsed_dest_ip_addr, parsed_optional.value, parsed_src_port,
      parsed_dest_port);

  free(udp_datagram.value);
  free(ip_datagram.value);
  free(ppp_frame.value);
  free(mac_frame.value);
  free(parsed_udp_datagram.value);
  free(parsed_ip_datagram.value);
  free(parsed_ppp_frame.value);
  free(parsed_mac_frame.value);
  if (parsed_optional.size > 0) {
    free(parsed_optional.value);
  }
  free(parsed_src_mac_addr.value);
  free(parsed_dest_mac_addr.value);

  original_data.value =
      "another test message which\t is processed into \aTCP datagram,\n IPv4 "
      "datagram, PPP frame\n and stuffed by zero-\bbit. After those works,\n "
      "it will be parsed into its\n\t original shape. Hope a good luck.\n\n";
  original_data.size = 199;

  send_tcp(original_data, &tcp_datagram, src_port, dest_port, tcp_id, ack_value,
           URG, ACK, PSH, RST, SYN, FIN, window, urg_ptr, optional, src_ip_addr,
           dest_ip_addr);
  send_ip_v4(tcp_datagram, &ip_datagram, diff_serv, ip_id, MF, DF,
             ip_offset_byte, TTL, ip_protocol, src_ip_addr, dest_ip_addr,
             optional);
  send_ppp(ip_datagram, &ppp_frame, ppp_protocol);
  bit_stuffing(ppp_frame, &bit_stuffing_stream);
  parse_bit_stuffing(bit_stuffing_stream, &parsed_bit_stuffing_stream);
  parse_ppp(parsed_bit_stuffing_stream, &parsed_ppp_frame,
            &parsed_ppp_protocol);
  parse_ip_v4(parsed_ppp_frame, &parsed_ip_datagram, &parsed_version,
              &parsed_header_byte, &parsed_diff_serv, &parsed_total_byte,
              &parsed_ip_id, &parsed_MF, &parsed_DF, &parsed_ip_offset_byte,
              &parsed_TTL, &parsed_protocol, &parsed_src_ip_addr,
              &parsed_dest_ip_addr, &parsed_optional);
  parse_tcp(parsed_ip_datagram, &parsed_tcp_datagram, &parsed_src_port,
            &parsed_dest_port, &parsed_tcp_id, &parsed_ack_value,
            &parsed_tcp_offset_byte, &parsed_URG, &parsed_ACK, &parsed_PSH,
            &parsed_RST, &parsed_SYN, &parsed_FIN, &parsed_window,
            &parsed_urg_ptr, &parsed_optional, src_ip_addr, dest_ip_addr);

  printf(
      "Parsed data: %s\nSize: %d\n\nPPP...\nProtocol: %d\n\nIP...\nVersion: "
      "%d\nHeader byte: %d\nDiffServ: %d\nTotal byte: %d\nID: %d\nMF: %d\nDF: "
      "%d\nOffset byte: %d\nTTL: %d\nProtocol: %d\nSource IP address: "
      "%x\nDestination IP address: %x\nOptional data: %s\n\nTCP...\nSource "
      "port: %x\nDestination port: %x\nID: "
      "%x\nACK value: %x\nHeader byte: %d\nURG: %d\nACK: %d\nPSH: %d\nRST: "
      "%d\nSYN: %d\nFIN: %d\nWindow: %d\nURG pointer: %x\nOptional data: "
      "%s\n\n\n",
      parsed_tcp_datagram.value, parsed_tcp_datagram.size, parsed_ppp_protocol,
      parsed_version, parsed_header_byte, parsed_diff_serv, parsed_total_byte,
      parsed_tcp_id, parsed_MF, parsed_DF, parsed_tcp_offset_byte, parsed_TTL,
      parsed_protocol, parsed_src_ip_addr, parsed_dest_ip_addr,
      parsed_optional.value, parsed_src_port, parsed_dest_port, parsed_tcp_id,
      parsed_ack_value, parsed_tcp_offset_byte, parsed_URG, parsed_ACK,
      parsed_PSH, parsed_RST, parsed_SYN, parsed_FIN, parsed_window,
      parsed_urg_ptr, parsed_optional.value);

  free(tcp_datagram.value);
  free(ip_datagram.value);
  free(ppp_frame.value);
  free(bit_stuffing_stream.value);
  free(parsed_bit_stuffing_stream.value);
  free(parsed_ip_datagram.value);
  free(parsed_ppp_frame.value);
  if (parsed_optional.size > 0) {
    free(parsed_optional.value);
  }
}