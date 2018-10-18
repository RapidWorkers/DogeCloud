#pragma once
#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef struct {
			unsigned char opCode;//operation code -> to classify packet
			char SessionKey[64];
			unsigned char selectedMenu;
	} cs_MenuSelect;

	typedef struct {
			unsigned char opCode;
			unsigned char selectedMenu;//contains user select
			unsigned char statusCode;//0 = fail, 1 = success
	} sc_MenuSelectResp;

#pragma pack(pop) //revert back to default
#ifdef __cplusplus
}
#endif