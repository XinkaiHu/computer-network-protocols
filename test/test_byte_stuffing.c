#include <stdio.h>
#include <stdlib.h>

#include "../src/ppp.h"
#include "../src/stuffing.h"
#include "../src/util.h"

int main(void) {
  data_t original_data = {.value =
                              "a simple test message for byte stuffing data",
                          .size = 48};
  PPP_PROTOCOL protocol = PPP_PROTOCOL_NCD;

  data_t ppp_frame;
  data_t ppp_frame_hex;

  data_t ppp_stuffing;
  data_t parsed_stuffing;

  data_t parsed_data;
  PPP_PROTOCOL parsed_protocal;

  send_ppp(original_data, &ppp_frame, protocol);
  byte_stuffing(ppp_frame, &ppp_stuffing);
  parse_byte_stuffing(ppp_stuffing, &parsed_stuffing);
  parse_ppp(parsed_stuffing, &parsed_data, &parsed_protocal);

  char_to_hex(ppp_frame, &ppp_frame_hex);

  printf("Point-to-Point frame: %s\nSize: %d\n\n", ppp_frame_hex.value,
         ppp_frame.size);
  printf("Parsed data: %s\nSize: %d\nProtocol: %d\n\n", parsed_data.value,
         parsed_data.size, parsed_protocal);

  free(ppp_frame.value);
  free(ppp_frame_hex.value);
  free(parsed_data.value);
  free(ppp_stuffing.value);
  free(parsed_stuffing.value);
}