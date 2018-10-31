#pragma once

//Packet opCode Definition
#define OP_CS_MENUSELECT 145
#define OP_SC_MENUSELECTRESP 146

#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef union {
		struct {
			unsigned long opCode;//operation code -> to classify packet
			unsigned long dataLen;
			unsigned char SessionKey[32];
			unsigned char selectedMenu;
		} Data;
		char buf[41];
	} cs_MenuSelect;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char selectedMenu;//contains user select
			unsigned char statusCode;//0 = fail, 1 = success
		} Data;
		char buf[10];
	} sc_MenuSelectResp;

#pragma pack(pop) //revert back to default
#ifdef __cplusplus
}
#endif