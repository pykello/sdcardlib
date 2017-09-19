
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

/*
 * SD Host Controller Spec v4.20, Page 50.
 * Cat. C, Offset 024h.
 */
struct PresentStateRegister {
	int commandInhibitCmd : 1;
	int commandInhibitDat : 1;
	int datLineActive : 1;
	int reTuningRequest : 1;
	int lineSignalLevel7to4 : 4;
	int writeTransferActive : 1;
	int readTransferActive : 1;
	int bufferWriteEnable : 1;
	int bufferReadEnable : 1;
	int reserved1 : 4;
	int cardInserted : 1;
	int cardStateStable : 1;
	int cardDetectPinLevel : 1;
	int writeProtectSwitchPinLevel : 1;
	int lineSignalLevel3to0 : 4;
	int cmdLineSignalLevel : 1;
	int hostRegularVoltageStable : 1;
	int reserved2 : 1;
	int commandNotIssuedByError : 1;
	int subCommandStatus : 1;
	int inDormantState : 1;
	int laneSync : 1;
	int uhs2IfDetection : 1;
};

struct __attribute__((__packed__)) SDRegisterMap {
	char reserved1[14];
	struct CommandRegister commandRegister;
	char reserved2[20];
	struct PresentStateRegister presentStateRegister;
};
