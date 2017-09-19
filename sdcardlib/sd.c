
#include "sdcardlib/sd.h"

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
 * SD Host Controller Spec v4.20, Page 45.
 */
struct CommandRegister {
	enum {
		RESP_NONE = 0,
		RESP_LEN136 = 1,
		RESP_LEN48 = 2,
		RESP_LEN48_CHECK_BUSY = 3
	} response_type_select : 2;
	/* 0 Main Command, 1 Sub Command. */
	int sub_command_flag : 1;
	/* Check CRC field in the response. */
	int command_crc_check_enable : 1;
	/* Check index field in the response to see it matches command index. */
	int command_index_check_enable : 1;
	/*
	 * 0 is used for (1) commands using only CMD line, (2) Commands with no data
	 * transfer but using busy signal on DAT[0] line, (3) Resume command.
	 */
	int data_present_select : 1;
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
	int reserved : 2;
};

/*
 * SD Host Controller Spec v4.20, Page 50.
 * Cat. C, Offset 024h.
 */
struct PresentStateRegister {
	int command_inhibit_cmd : 1;
	int command_inhibit_dat : 1;
	int dat_line_active : 1;
	int retuning_request : 1;
	int line_signal_level_7_to_4 : 4;
	int write_transfer_active : 1;
	int read_transfer_active : 1;
	int buffer_write_enable : 1;
	int buffer_read_enable : 1;
	int reserved_1 : 4;
	int card_inserted : 1;
	int card_state_stable : 1;
	int card_detect_pin_level : 1;
	int write_protect_switch_pin_level : 1;
	int line_signal_level_3_to_0 : 4;
	int cmd_line_signal_level : 1;
	int host_regular_voltage_stable : 1;
	int reserved_2 : 1;
	int command_not_issued_by_error : 1;
	int sub_command_status : 1;
	int in_dormant_state : 1;
	int lane_sync : 1;
	int uhs2_if_detection : 1;
};

static volatile struct __attribute__((__packed__)) {
	char reserved_1[14];
	struct CommandRegister command_register;
	char reserved_2[20];
	struct PresentStateRegister present_state_register;
} *sd_register_map;


void set_sd_register_map_base(void *address) {
	sd_register_map = address;
}

