
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

/*
 * Based on:
 *   SD Host Controller Simplified Specification Version 2.00
 *   3.7.1.1. The sequence to issue a SD Command
 */
static void
issue_sd_command() {

	/* (1) Check Command Inhibit (CMD) until it is free. */
	// while (read32(SDHC_BASE + EMMC_STATUS) & COMMAND_INHIBIT_CMD);

	/* (2) Issue the command with the busy signal? */

}

void c_entry() {
 print_uart0("Hello world!\n");
 set_sd_register_map_base((void *) 0x10005000);

}
