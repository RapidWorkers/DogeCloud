#pragma once
#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef union {//use union to send packets network easily
		struct {//manipulate packet structure in program
			unsigned char opCode;//operation code -> to classify packet
			char UserSessionKey[64];
			char UserFileServerAuthKey[64];//random sha256 hash
		} Packet;
		char buffer[125];//send this buffer to socket and first byte of buffer means opCode for classifying packet
	} sf_AuthUser;

	typedef union {
		struct {
			unsigned char opCode;//operation code -> to classify packet
			unsigned char statusCode; //0 = fail, 1 = success
		} Packet;
		char buffer[2];
	} sf_AuthUserResp;

#pragma pack(pop)
#ifdef __cplusplus
}
#endif