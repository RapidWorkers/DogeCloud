#include <stdio.h>
#include "../DC_Common/DC_Common.h"
#pragma comment(lib, "DC_Common.lib")


int main() {
	printf("============ 1. DLL Call Test ============\n");
	myFunc();
	printf("\n============ 2. LEA Library DLL Call Test ============\n");
	testLEA();
	printf("\n============ 3. Testing Packet Structure from DLL ============\n");
	testPacketStructure();
	printf("\n\n============ 4. Testing SHA256 Library from DLL ============\n");
	testSHA256();
	printf("\n\n============ 5. Library Include Test ============\n");

	cs_LoginStart test;
	cs_LoginStart temp;

	test.Packet.opCode = 102;
	test.Packet.clientVersion = 123456789;
	//memcpy(temp.buffer, test.buffer, sizeof(test));
	//memcpy(bufferMove, test.buffer, sizeof(test));
	//test.Packet.opCode = 103;

	for (int i = 0; i < 5; i++) {
		temp.buffer[i] = test.buffer[i];
	}

	printf("ORG: opCode: %d clientVersion: %d raw0: %X raw1: %X raw2: %X raw3: %X raw4: %X buf_addr: %p\n", test.Packet.opCode, test.Packet.clientVersion, test.buffer[0], test.buffer[1], test.buffer[2], test.buffer[3], test.buffer[4], test.buffer);
	printf("RST: opCode: %d clientVersion: %d raw0: %X raw1: %X raw2: %X raw3: %X raw4: %X buf_addr: %p\n", temp.Packet.opCode, temp.Packet.clientVersion, temp.buffer[0], temp.buffer[1], temp.buffer[2], temp.buffer[3], temp.buffer[4], temp.buffer);

	system("pause");
}