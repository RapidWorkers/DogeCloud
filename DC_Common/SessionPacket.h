#pragma once

//Packet opCode Definition
#define OP_CS_LOGINSTART 139
#define OP_SC_LOGINSTARTRESP 140
#define OP_CS_LOGINACCOUNTDATA 141
#define OP_SC_LOGINDONERESP 142

#define OP_CS_LOGOUTSTART 143
#define OP_SC_LOGOUTDONE 144

#define OP_CS_REGISTERSTART 168
#define OP_SC_REGISTERSTARTRESP 169
#define OP_CS_REGISTERACCOUNTDATA 170
#define OP_SC_REGISTERDONE 171


#ifdef __cplusplus //determine if using cpp compiler
extern "C" {
#endif
#pragma pack(push, 1) //struct expands every 1 byte

	typedef union {//use union to send packets network easily
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned long clientVersion;//클라이언트 버전
		} Data;
		char buf[12];
	}cs_LoginStart;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode;//상태 코드
		} Data;
		char buf[9];
	}sc_LoginStartResp;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			char Username[100];//아이디
			char Password[100];//raw password
		} Data;
		char buf[208];
	}cs_LoginAccountData;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode;//0 = fail, 1 = success
			unsigned char sessionKey[32];//use random sha256 hash, if login has failled, fill this with 0
		} Data;
		char buf[41];
	}sc_LoginDoneResp;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char SessionKey[32];//세션키
		} Data;
		char buf[40];
	}cs_LogoutStart;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode;//0 = fail, 1 = success
		} Data;
		char buf[9];
	}sc_LogoutDone;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned long clientVersion;//0 = fail, 1 = success
		} Data;
		char buf[12];
	}cs_RegisterStart;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode;//상태 코드
		} Data;
		char buf[9];
	}sc_RegisterStartResp;

	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			char email[100];//이메일
			char Username[100];//아이디
			char Password[100];//raw password
		} Data;
		char buf[308];
	}cs_RegisterAccountData;
	
	typedef union {
		struct {
			unsigned long opCode;//opCode => 패킷 구분용
			unsigned long dataLen;//데이터 길이
			unsigned char statusCode;//0 = fail, 1 = dup id, 2 = success
			unsigned char sessionKey[32];//use random sha256 hash, if login has failled, fill this with 0
		} Data;
		char buf[41];
	}sc_RegisterDone;

#pragma pack(pop) //revert back to default
#ifdef __cplusplus
}
#endif