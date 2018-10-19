#pragma once
#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef struct {
			unsigned char opCode;//operation code -> to classify packet
			char SessionKey[32];
			unsigned char statusCode;//0 = fail, 1 = success
	} cs_ContatEditDone;

	typedef struct {
			unsigned char opCode;
			unsigned char statusCode;//0 = fail, 1 = success
	} sc_ContactEditDoneResp;

	typedef struct {
			unsigned char opCode;
			char SessionKey[32];
			unsigned char statusCode;//0 = fail, 1 = success
	} cs_MemoEditDone;

	typedef struct {
			unsigned char opCode;
			unsigned char statusCode;//0 = fail, 1 = success
	} sc_MemoEditDoneResp;


#pragma pack(pop) //revert back to default
#ifdef __cplusplus
}
#endif