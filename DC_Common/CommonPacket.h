#pragma once

#define OP_PINGPACKET 22

#ifdef __cplusplus //C++ 컴파일러가 이 코드를 컴파일 하는지 검사
extern "C" {
#endif
#pragma pack(push, 1) //구조체를 1 byte씩 확장하도록 -> 패딩 바이트 없음

typedef union {
	unsigned long opCode;
	unsigned long ping;
	char buf[8];
} PingPacket;

#pragma pack(pop) //구조체 설정을 기본값으로 되돌림
#ifdef __cplusplus
}
#endif