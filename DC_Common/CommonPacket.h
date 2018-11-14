#pragma once

#define OP_PINGPACKET 22

#ifdef __cplusplus //C++ �����Ϸ��� �� �ڵ带 ������ �ϴ��� �˻�
extern "C" {
#endif
#pragma pack(push, 1) //����ü�� 1 byte�� Ȯ���ϵ��� -> �е� ����Ʈ ����

typedef union {
	unsigned long opCode;
	unsigned long ping;
	char buf[8];
} PingPacket;

#pragma pack(pop) //����ü ������ �⺻������ �ǵ���
#ifdef __cplusplus
}
#endif