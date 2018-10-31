#pragma once

//Packet opCode Definition
#define OP_CS_CONTACTEDIT 151
#define OP_SC_CONTACTEDITDONE 152
#define OP_CS_MEMOEDITDONE 153
#define OP_SC_MEMOEDITDONERESP 154

#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte
	typedef union {
		struct {
			unsigned long opCode;//operation code -> to classify packet
			unsigned long dataLen;
			unsigned char SessionKey[32];
			unsigned char statusCode;//0 = fail, 1 = success
		} Data;
		char buf[41];
	} cs_ContactEditDone;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode;//0 = fail, 1 = success
		} Data;
		char buf[9];
	} sc_ContactEditDoneResp;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char SessionKey[32];
			unsigned char statusCode;//0 = fail, 1 = success
		} Data;
		char buf[41];
	} cs_MemoEditDone;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode;//0 = fail, 1 = success
		} Data;
		char buf[9];
	} sc_MemoEditDoneResp;

#pragma pack(pop) //revert back to default
#ifdef __cplusplus
}
#endif