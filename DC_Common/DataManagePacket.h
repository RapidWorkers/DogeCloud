#pragma once

//��Ŷ opCode ����
#define OP_CS_CONTACTEDIT 151
#define OP_SC_CONTACTEDITDONE 152
#define OP_CS_MEMOEDITDONE 153
#define OP_SC_MEMOEDITDONERESP 154

#define OP_CS_DOWNLOADUSERINFOREQ 80
#define OP_SC_DOWNLOADUSERINFORESP 81

#ifdef __cplusplus //C++ �����Ϸ��� �� �ڵ带 ������ �ϴ��� �˻�
extern "C" {
#endif
#pragma pack(push, 1) //����ü�� 1 byte�� Ȯ���ϵ��� -> �е� ����Ʈ ����
	typedef union {
		struct {
			unsigned long opCode;//opCode => ��Ŷ ���п�
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

#pragma pack(pop) //����ü ������ �⺻������ �ǵ���
#ifdef __cplusplus
}
#endif