#pragma once
#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef union {//use union to send packets network easily
		struct {//manipulate packet structure in program
			unsigned char opCode;//operation code -> to classify packet
			int clientVersion;
		} Packet;
		char buffer[5];//send this buffer to socket and first byte of buffer means opCode for classifying packet
	}cs_LoginStart;

	typedef union  {
		struct {
			unsigned char opCode;
			char statusCode;
		} Packet;
		char buffer[2];
	}sc_LoginStartResp;

	typedef union {
		struct {
			unsigned char opCode;
			char Username[100];
			char Password[100];//raw password
		} Packet;
		char buffer[201];
	}cs_LoginAccountData;

	typedef union {
		struct {
			unsigned char opCode;
			char statusCode;//0 = fail, 1 = success
			char SessionKey[64];//use random sha256 hash, if login has failled, fill this with 0
		} Packet;
		char buffer[65];
	}sc_LoginDoneResp;

	typedef union {
		struct {
			unsigned char opCode;
			char SessionKey[64];
		} Packet;
		char buffer[65];
	}cs_LogoutStart;

	typedef union {
		struct {
			unsigned char opCode;
			char statusCode;//0 = fail, 1 = success
		} Packet;
		char buffer[2];
	}sc_LogoutDone;
	

#pragma pack(pop) //revert back to default
#ifdef __cplusplus
}
#endif