#include <stdio.h>
#include <stdlib.h>

#include "../src/ppp.h"
#include "../src/util.h"

int main(void) {
  data_t original_data = {.value =
                              "a simple test message for framing and parsing "
                              "of Point-to-Point Protocol",
                          .size = 73};
  PPP_PROTOCOL protocol = PPP_PROTOCOL_NCD;

  data_t ppp_frame;
  data_t ppp_frame_hex;

  data_t parsed_data;
  PPP_PROTOCOL parsed_protocal;

  send_ppp(original_data, &ppp_frame, protocol);
  parse_ppp(ppp_frame, &parsed_data, &parsed_protocal);

  char_to_hex(ppp_frame, &ppp_frame_hex);

  printf("Point-to-Point frame: %s\nSize: %d\n\n", ppp_frame_hex.value,
         ppp_frame.size);
  printf("Parsed data: %s\nSize: %d\nProtocol: %d\n\n", parsed_data.value,
         parsed_data.size, parsed_protocal);

  free(ppp_frame.value);
  free(ppp_frame_hex.value);
  free(parsed_data.value);
}