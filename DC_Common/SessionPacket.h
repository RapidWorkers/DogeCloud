#pragma once
#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef struct {//use union to send packets network easily
		unsigned char opCode;//operation code -> to classify packet
		int clientVersion;
	}cs_LoginStart;

	typedef struct {
		unsigned char opCode;
		char statusCode;
	}sc_LoginStartResp;

	typedef struct {
		unsigned char opCode;
		char Username[100];
		char Password[100];//raw password
	}cs_LoginAccountData;

	typedef struct {
		unsigned char opCode;
		char statusCode;//0 = fail, 1 = success
		char sessionKey[32];//use random sha256 hash, if login has failled, fill this with 0
	}sc_LoginDoneResp;

	typedef struct {
		unsigned char opCode;
		char SessionKey[32];
	}cs_LogoutStart;

	typedef struct {
		unsigned char opCode;
		char statusCode;//0 = fail, 1 = success
	}sc_LogoutDone;
	

#pragma pack(pop) //revert back to default
#ifdef __cplusplus
}
#endif