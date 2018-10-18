#pragma once
#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef struct { //use union to send packets network easily
			unsigned char opCode;//operation code -> to classify packet
			char UserSessionKey[32];
			char UserFileServerAuthKey[32];//random sha256 hash
	} sf_AuthUser;

	typedef struct {
			unsigned char opCode;//operation code -> to classify packet
			unsigned char statusCode; //0 = fail, 1 = success
	} sf_AuthUserResp;

#pragma pack(pop)
#ifdef __cplusplus
}
#endif