#include <stdio.h>

#include "src/ethernet_v2.h"
#include "src/ip.h"
#include "src/ppp.h"
#include "src/stuffing.h"
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

  uint8_t diff_serv = 1;
  uint16_t id = 1;
  uint8_t MF = 1;
  uint8_t DF = 0;
  uint16_t offset_byte = 24;
  uint8_t TTL = 4;
  IPv4_PROTOCOL_TYPE ip_protocol = IPv4_PROTOCOL_TYPE_UDP;
  data_t optional = {.value = "Optional configurations.", .size = 25};

  PPP_PROTOCOL ppp_protocol = PPP_PROTOCOL_IPD;

  data_t src_mac_addr = {.value = "012345", .size = 6};
  data_t dest_mac_addr = {.value = "FEDCBA", .size = 6};
  ETHERNET_V2_TYPE type = ETHERNET_V2_TYPE_PPPoE;

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

  uint16_t parsed_src_port;
  uint16_t parsed_dest_port;
  uint32_t parsed_src_ip_addr;
  uint32_t parsed_dest_ip_addr;

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
  data_t parsed_optional;

  PPP_PROTOCOL parsed_ppp_protocol;

  ETHERNET_V2_TYPE parsed_type;
  data_t parsed_src_mac_addr;
  data_t parsed_dest_mac_addr;

  send_udp(original_data, &udp_datagram, src_port, dest_port, src_ip_addr,
           dest_ip_addr);
  send_ip_v4(udp_datagram, &ip_datagram, diff_serv, id, MF, DF, offset_byte,
             TTL, ip_protocol, src_ip_addr, dest_ip_addr, optional);
  send_ppp(ip_datagram, &ppp_frame, ppp_protocol);
  send_ethernet_v2(ppp_frame, &mac_frame, dest_mac_addr, src_mac_addr, type);
  parse_ethernet_v2(mac_frame, &parsed_mac_frame, &parsed_dest_mac_addr,
                    &parsed_src_mac_addr, &parsed_type);
  parse_ppp(parsed_mac_frame, &parsed_ppp_frame, &parsed_ppp_protocol);
  parse_ip_v4(parsed_ppp_frame, &parsed_ip_datagram, &parsed_version,
              &parsed_header_byte, &parsed_diff_serv, &parsed_total_byte,
              &parsed_id, &parsed_MF, &parsed_DF, &parsed_offset_byte,
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
      parsed_total_byte, parsed_id, parsed_MF, parsed_DF, parsed_offset_byte,
      parsed_TTL, parsed_protocol, parsed_src_ip_addr, parsed_dest_ip_addr,
      parsed_optional.value, parsed_src_port, parsed_dest_port);

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
      "another test message which\t is processed into \aUDP datagram,\n IPv4 "
      "datagram, PPP frame\n and stuffed by zero-\bbit. After those works,\n "
      "it "
      "will be parsed into its\n\t original shape. Hope a good luck.\n\n";
  original_data.size = 199;

  send_udp(original_data, &udp_datagram, src_port, dest_port, src_ip_addr,
           dest_ip_addr);
  send_ip_v4(udp_datagram, &ip_datagram, diff_serv, id, MF, DF, offset_byte,
             TTL, ip_protocol, src_ip_addr, dest_ip_addr, optional);
  send_ppp(ip_datagram, &ppp_frame, ppp_protocol);
  bit_stuffing(ppp_frame, &bit_stuffing_stream);
  parse_bit_stuffing(bit_stuffing_stream, &parsed_bit_stuffing_stream);
  parse_ppp(parsed_bit_stuffing_stream, &parsed_ppp_frame,
            &parsed_ppp_protocol);
  parse_ip_v4(parsed_ppp_frame, &parsed_ip_datagram, &parsed_version,
              &parsed_header_byte, &parsed_diff_serv, &parsed_total_byte,
              &parsed_id, &parsed_MF, &parsed_DF, &parsed_offset_byte,
              &parsed_TTL, &parsed_protocol, &parsed_src_ip_addr,
              &parsed_dest_ip_addr, &parsed_optional);
  parse_udp(parsed_ip_datagram, &parsed_udp_datagram, &parsed_src_port,
            &parsed_dest_port, parsed_src_ip_addr, parsed_dest_ip_addr);

  printf(
      "Parsed data: %s\nSize: %d\n\nPPP...\nProtocol: %d\n\nIP...\nVersion: "
      "%d\nHeader byte: %d\nDiffServ: %d\nTotal byte: %d\nID: %d\nMF: %d\nDF: "
      "%d\nOffset byte: %d\nTTL: %d\nProtocol: %d\nSource IP address: "
      "%x\nDestination IP address: %x\nOptional data: %s\n\nUDP...\nSource "
      "port: %x\nDestination port: %x\n\n\n",
      parsed_udp_datagram.value, parsed_udp_datagram.size, parsed_ppp_protocol,
      parsed_version, parsed_header_byte, parsed_diff_serv, parsed_total_byte,
      parsed_id, parsed_MF, parsed_DF, parsed_offset_byte, parsed_TTL,
      parsed_protocol, parsed_src_ip_addr, parsed_dest_ip_addr,
      parsed_optional.value, parsed_src_port, parsed_dest_port);

  free(udp_datagram.value);
  free(ip_datagram.value);
  free(ppp_frame.value);
  free(bit_stuffing_stream.value);
  free(parsed_bit_stuffing_stream.value);
  free(parsed_udp_datagram.value);
  free(parsed_ip_datagram.value);
  free(parsed_ppp_frame.value);
  if (parsed_optional.size > 0) {
    free(parsed_optional.value);
  }
}