
#include "sdcardlib/sd.h"

/* integer types */
typedef __signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned int size_t;


/*
 * Physical Layer Spec v2.00, Page 49.
 */
enum CommandIndex {
	/* Basic commands */
	GO_IDLE_STATE = 0,
	ALL_SEND_CID = 2,
	SEND_RELATIVE_ADDR = 3,
	SET_DSR = 4,
	SELECT_DESELECT_CARD = 7,
	SEND_IF_COND = 8,
	SEND_CSD = 9,
	SEND_CID = 10,
	STOP_TRANSMISSION = 12,
	SEND_STATUS = 13,
	GO_INACTIVE_STATE = 15,
	/* Block-oriented read commands */
	SET_BLOCKLEN = 16,
	READ_SINGLE_BLOCK = 17,
	READ_MULTIPLE_BLOCKS = 18,
	/* Block-oriented write commands */
	WRITE_SINGLE_BLOCK = 24,
	WRITE_MULTIPLE_BLOCKS = 25,
	PROGRAM_CSD = 27
};

/*
 * Physical Layer Spec v2.00, Page 58
 */
enum ResponseType {
	RESP_TYPE_NONE,
	RESP_TYPE_R1,  /* Normal response command */
	RESP_TYPE_R1b, /* R1 with busy signal */
	RESP_TYPE_R2,  /* CID, CSD register */
	RESP_TYPE_R3,  /* OCR register */
	RESP_TYPE_R6,  /* Published RCA response */
	RESP_TYPE_R7   /* Card interface condition */
};

/*
 * SD Host Controller Spec v4.20, Page 45.
 */
struct CommandRegister {
	enum {
		RESP_NONE = 0,
		RESP_LEN136 = 1,
		RESP_LEN48 = 2,
		RESP_LEN48_CHECK_BUSY = 3
	} response_type : 2;
	/* 0 Main Command, 1 Sub Command. */
	uint8_t sub_command_flag : 1;
	/* Check CRC field in the response. */
	uint8_t command_crc_check_enable : 1;
	/* Check index field in the response to see it matches command index. */
	uint8_t command_index_check_enable : 1;
	/*
	 * 0 is used for (1) commands using only CMD line, (2) Commands with no data
	 * transfer but using busy signal on DAT[0] line, (3) Resume command.
	 */
	uint8_t data_present : 1;
	/* 11b Abort, 10b Resume, 01b Suspend, 00 Other commands */
	enum {
		CMD_NORMAL = 0,
		CMD_SUSPEND = 1,
		CMD_RESUME = 2,
		CMD_ABORT = 3
	} command_type : 2;
	/*
	 * Command number that is specified in bits 45-40 of the Command-Format in
	 * the Physical Layer Specification and SDIO Card Specification.
	 */
	enum CommandIndex command_index : 6;
	uint8_t reserved : 2;
};

const struct CommandRegister command_idx_to_reg[] = {
	[GO_IDLE_STATE] = {RESP_NONE, 0, 0, 0, 0, CMD_NORMAL, GO_IDLE_STATE, 0},
	// [ALL_SEND_CID] = {RESP_TYPE_R2},
	// [SEND_RELATIVE_ADDR] = {RESP_TYPE_R6},
	// [SET_DSR] = {RESP_TYPE_NONE},
	// [SELECT_DESELECT_CARD] = {RESP_TYPE_R1b},
	// [SEND_IF_COND] = {RESP_TYPE_R7},
	// [SEND_CSD] = {RESP_TYPE_R2},
	// [SEND_CID] = {RESP_TYPE_R2},
	// [STOP_TRANSMISSION] = {RESP_TYPE_R1b},
	// [SEND_STATUS] = {RESP_TYPE_R1},
	// [GO_INACTIVE_STATE] = {RESP_TYPE_NONE},
	// [SET_BLOCKLEN] = {RESP_TYPE_R1},
	// [READ_SINGLE_BLOCK] = {RESP_TYPE_R1},
	// [READ_MULTIPLE_BLOCKS] = {RESP_TYPE_R1},
	// [WRITE_SINGLE_BLOCK] = {RESP_TYPE_R1},
	// [WRITE_MULTIPLE_BLOCKS] = {RESP_TYPE_R1},
	// [PROGRAM_CSD] = {RESP_TYPE_R1}
};

/*
 * SD Host Controller Spec v4.20, Page 50.
 * Cat. C, Offset 024h.
 */
struct PresentStateRegister {
	uint8_t command_inhibit_cmd : 1;
	uint8_t command_inhibit_dat : 1;
	uint8_t dat_line_active : 1;
	uint8_t retuning_request : 1;
	uint8_t line_signal_level_7_to_4 : 4;
	uint8_t write_transfer_active : 1;
	uint8_t read_transfer_active : 1;
	uint8_t buffer_write_enable : 1;
	uint8_t buffer_read_enable : 1;
	uint8_t reserved_1 : 4;
	uint8_t card_inserted : 1;
	uint8_t card_state_stable : 1;
	uint8_t card_detect_pin_level : 1;
	uint8_t write_protect_switch_pin_level : 1;
	uint8_t line_signal_level_3_to_0 : 4;
	uint8_t cmd_line_signal_level : 1;
	uint8_t host_regular_voltage_stable : 1;
	uint8_t reserved_2 : 1;
	uint8_t command_not_issued_by_error : 1;
	uint8_t sub_command_status : 1;
	uint8_t in_dormant_state : 1;
	uint8_t lane_sync : 1;
	uint8_t uhs2_if_detection : 1;
};

/*
 * SD Host Controller Spec v4.20, Page 76.
 * Cat. C, Offset 024h.
 */
struct NormalInterruptStatusRegister {
	uint8_t command_complete : 1;
	uint8_t transfer_complete : 1;
	uint8_t block_gap_event : 1;
	uint8_t dma_interrupt : 1;
	uint8_t buffer_write_ready : 1;
	uint8_t buffer_read_ready : 1;
	uint8_t card_insertion : 1;
	uint8_t card_removal : 1;
	uint8_t card_interrupt : 1;
	uint8_t int_a : 1;
	uint8_t int_b : 1;
	uint8_t int_c : 1;
	uint8_t retuning_event : 1;
	uint8_t fx_event : 1;
	uint8_t reserved : 1;
	uint8_t error_interrupt : 1;
};

/*
 * SD Host Controller Spec v4.20, Page 32.
 */
static volatile struct __attribute__((__packed__)) {
	char reserved_1[8];
	uint32_t argument;
	char reserved_2[4];
	struct CommandRegister command;
	char reserved_3[20];
	struct PresentStateRegister present_state;
	char reserved_4[8];
	struct NormalInterruptStatusRegister normal_interrupt_status;
} *sd_registers;


static void issue_sd_command(enum CommandIndex idx, uint32_t arg);
static void complete_sd_command(void);

void set_sd_register_map_base(void *address) {
	sd_registers = address;
}

/*
 * SD Host Controller Spec v4.20, Page 91.
 */
static void
issue_sd_command(enum CommandIndex idx, uint32_t arg) {
	struct CommandRegister reg = command_idx_to_reg[idx];

	while (sd_registers->present_state.command_inhibit_cmd);

	if (reg.response_type == RESP_LEN48_CHECK_BUSY &&
		reg.command_type != CMD_ABORT) {
		while (sd_registers->present_state.command_inhibit_dat);
	}

	sd_registers->argument = arg;
	sd_registers->command = reg;

	complete_sd_command();
}

/*
 * SD Host Controller Spec v4.20, Page 93.
 */
static void
complete_sd_command(void) {
	int cntr = 0;
	const int TIMEOUT = 1000000;

	while (!sd_registers->normal_interrupt_status.command_complete &&
		   !sd_registers->normal_interrupt_status.error_interrupt) {
		if (++cntr == TIMEOUT)
			break;
	}

	sd_registers->normal_interrupt_status.command_complete = 1;

	if (sd_registers->normal_interrupt_status.error_interrupt) {
		/* TODO: Error handling. */
		return;
	}

	/* TODO: Read response data. */

	/* TODO: Command with transfer complete int? */
}


/*
 * memcmp copies n bytes from the source buffer to the target buffer. It returns
 * the pointer to the target.
 */
void *memcpy(void *target, const void *source, size_t n)
{
	char *target_buffer = (char *) target;
	char *source_buffer = (char *) source;
	size_t i = 0;

	for (i = 0; i < n; i++)
		target_buffer[i] = source_buffer[i];

	return target;
}
