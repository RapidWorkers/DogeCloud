#pragma once
#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
		} Data;
		char buf[8];
	} fs_RegisterFileServer;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode; //0 = fail, 1 = success
		} Data;
		char buf[9];
	} sf_RegisterFileServerResp;

	typedef union {//use union to send packets network easily
		struct {
			unsigned long opCode;//operation code -> to classify packet
			unsigned long dataLen;
			unsigned char UserSessionKey[32];
			unsigned char UserFileServerAuthKey[32];//random sha256 hash
		} Data;
		char buf[72];
	} sf_AuthUser;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char statusCode; //0 = fail, 1 = success
		}Data;
		char buf[9];
	} fs_AuthUserResp;

#pragma pack(pop)
#ifdef __cplusplus
}
#endif