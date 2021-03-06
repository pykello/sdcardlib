
#include "sdcardlib/sd.h"

typedef int int32_t;
typedef unsigned int uint32_t;

volatile uint32_t * const UART0DR = (uint32_t *) 0x101f1000;

#define SD_REG_PRESENT_STATUS 0x24;
#define COMMAND_INHIBIT_CMD 0x1;
#define COMMAND_INHIBIT_DAT 0x2;
 
void print_uart0(const char *s) {
 while(*s != '\0') { /* Loop until end of string */
 *UART0DR = (unsigned int)(*s); /* Transmit char */
 s++; /* Next char */
 }
}

void c_entry() {
 print_uart0("Hello world!\n");
 set_sd_register_map_base((void *) 0x10005000);

}
