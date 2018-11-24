#pragma once

//��Ŷ opCode ����
#define OP_CS_PERSONALDBEDITDONE 151
#define OP_SC_PERSONALDBEDITDONERESP 152

#define OP_CS_DOWNLOADUPERSONALDBREQ 80
#define OP_SC_DOWNLOADPERSONALDBRESP 81

#ifdef __cplusplus //C++ �����Ϸ��� �� �ڵ带 ������ �ϴ��� �˻�
extern "C" {
#endif
#pragma pack(push, 1) //����ü�� 1 byte�� Ȯ���ϵ��� -> �е� ����Ʈ ����
	typedef union {
		struct {
			unsigned long opCode;//opCode => ��Ŷ ���п�
			unsigned long dataLen;//������ ����
			unsigned char hash[32];//�ؽ���
		} Data;
		char buf[40];
	} cs_PersonalDBEditDone;

	typedef union {
		struct {
			unsigned long opCode;//opCode => ��Ŷ ���п�
			unsigned long dataLen;//������ ����
			unsigned char statusCode;//0 = fail, 1 = success
		} Data;
		char buf[9];
	} sc_PersonalDBEditDoneResp;

	typedef union {
		struct {
			unsigned long opCode;//opCode => ��Ŷ ���п�
			unsigned long dataLen;//������ ����
		} Data;
		char buf[8];
	} cs_DownloadPersonalDBReq;

	typedef union {
		struct {
			unsigned long opCode;//opCode => ��Ŷ ���п�
			unsigned long dataLen;//������ ����
			unsigned char hash[32];//�ؽ���
	} Data;
		char buf[40];
	} sc_DownloadPersonalDBResp;

#pragma pack(pop) //����ü ������ �⺻������ �ǵ���
#ifdef __cplusplus
}
#endif