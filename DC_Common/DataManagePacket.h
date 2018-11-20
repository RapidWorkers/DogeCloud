#pragma once

//패킷 opCode 정의
#define OP_CS_CONTACTEDIT 151
#define OP_SC_CONTACTEDITDONE 152
#define OP_CS_MEMOEDITDONE 153
#define OP_SC_MEMOEDITDONERESP 154

#define OP_CS_DOWNLOADUSERINFOREQ 80
#define OP_SC_DOWNLOADUSERINFORESP 81

#ifdef __cplusplus //C++ 컴파일러가 이 코드를 컴파일 하는지 검사
extern "C" {
#endif
#pragma pack(push, 1) //구조체를 1 byte씩 확장하도록 -> 패딩 바이트 없음
	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
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

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
		} Data;
		char buf[8];
	} cs_DownloadInfoReq;

	typedef union {
		struct {
			unsigned long opCode;
			unsigned long dataLen;
			unsigned char hash[32];
	} Data;
		char buf[40];
	} sc_DownloadInfoResp;

#pragma pack(pop) //구조체 설정을 기본값으로 되돌림
#ifdef __cplusplus
}
#endif