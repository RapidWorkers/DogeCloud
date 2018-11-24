#pragma once

//패킷 opCode 정의
#define OP_CS_PERSONALDBEDITDONE 151
#define OP_SC_PERSONALDBEDITDONERESP 152

#define OP_CS_DOWNLOADUPERSONALDBREQ 80
#define OP_SC_DOWNLOADPERSONALDBRESP 81

#ifdef __cplusplus //C++ 컴파일러가 이 코드를 컴파일 하는지 검사
extern "C" {
#endif
#pragma pack(push, 1) //구조체를 1 byte씩 확장하도록 -> 패딩 바이트 없음
	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char hash[32];//해쉬값
		} Data;
		char buf[40];
	} cs_PersonalDBEditDone;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode;//0 = fail, 1 = success
		} Data;
		char buf[9];
	} sc_PersonalDBEditDoneResp;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
		} Data;
		char buf[8];
	} cs_DownloadPersonalDBReq;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char hash[32];//해쉬값
	} Data;
		char buf[40];
	} sc_DownloadPersonalDBResp;

#pragma pack(pop) //구조체 설정을 기본값으로 되돌림
#ifdef __cplusplus
}
#endif