
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
 * SDHCSS V4.20, Page 45.
 */
struct CommandRegister {
	enum {
		RESP_NONE = 0,
		RESP_LEN136 = 1,
		RESP_LEN48 = 2,
		RESP_LEN48_CHECK_BUSY = 3
	} responseTypeSelect : 2;
	/* 0 Main Command, 1 Sub Command. */
	int subCommandFlag : 1;
	/* Check CRC field in the response. */
	int commandCrcCheckEnable : 1;
	/* Check index field in the response to see it matches command index. */
	int commandIndexCheckEnable : 1;
	/*
	 * 0 is used for (1) commands using only CMD line, (2) Commands with no data
	 * transfer but using busy signal on DAT[0] line, (3) Resume command.
	 */
	int dataPresentSelect : 1;
	/* 11b Abort, 10b Resume, 01b Suspend, 00 Other commands */
	enum {
		CMD_NORMAL = 0,
		CMD_SUSPEND = 1,
		CMD_RESUME = 2,
		CMD_ABORT = 3
	} commandType : 2;
	/*
	 * Command number that is specified in bits 45-40 of the Command-Format in
	 * the Physical Layer Specification and SDIO Card Specification.
	 */
	enum CommandIndex commandIndex : 6;
	int reserved : 2;
};
