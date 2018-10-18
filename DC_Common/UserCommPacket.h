#pragma once
#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef union {//use union to send packets network easily
		struct {//manipulate packet structure in program
			unsigned char opCode;//operation code -> to classify packet
			char SessionKey[64];
			unsigned char selectedMenu;
		} Packet;
		char buffer[66];//send this buffer to socket and first byte of buffer means opCode for classifying packet
	} cs_MenuSelect;

	typedef union {
		struct {
			unsigned char opCode;
			unsigned char selectedMenu;//contains user select
			unsigned char statusCode;//0 = fail, 1 = success
		} Packet;
		char buffer[3];
	} sc_MenuSelectResp;

#pragma pack(pop) //revert back to default
#ifdef __cplusplus
}
#endif