#include <stdio.h>
#include <stdlib.h>

#include "../src/ethernet_v2.h"
#include "../src/util.h"

int main(void) {
  data_t original_data = {.value =
                              "a simple test message for framing and parsing "
                              "of Ethernet v2 protocol",
                          .size = 70};
  data_t dest_addr = {.value = "12345", .size = 6};
  data_t src_addr = {.value = "67890", .size = 6};
  ETHERNET_V2_TYPE type = ETHERNET_V2_TYPE_IPv4;

  data_t mac_frame;
  data_t mac_frame_hex;

  data_t parsed_data;
  data_t parsed_desr_addr;
  data_t parsed_src_addr;
  ETHERNET_V2_TYPE parsed_type;

  send_ethernet_v2(original_data, &mac_frame, dest_addr, src_addr, ETHERNET_V2_TYPE_IPv4);
  parse_ethernet_v2(mac_frame, &parsed_data, &parsed_desr_addr,
                    &parsed_src_addr, &parsed_type);

  char_to_hex(mac_frame, &mac_frame_hex);

  printf("Ethernet v2 Mac frame: %s\nSize: %d\n\n", mac_frame_hex.value,
         mac_frame.size);
  printf(
      "Parsed data: %s\nSize: %d\nDestination address: %s\nSource address: "
      "%s\nType: %d\n\n",
      parsed_data.value, parsed_data.size, parsed_desr_addr.value,
      parsed_src_addr.value, parsed_type);

  free(mac_frame.value);
  free(mac_frame_hex.value);
  free(parsed_data.value);
  free(parsed_desr_addr.value);
  free(parsed_src_addr.value);
}