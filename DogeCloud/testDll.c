#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

	test.opCode = 102;
	test.clientVersion = 2100000000;

	char *buf = malloc(sizeof(cs_LoginStart));

	if (buf == NULL) {
		printf("FATAL ERROR! Memory Allocation Failled!!");
		exit(1);
	}

	memcpy(buf, &test, sizeof(cs_LoginStart));
	memcpy(&temp, buf, sizeof(cs_LoginStart));
	test.opCode = 103;

	printf("ORG: opCode: %d clientVersion: %d struct_addr: %p\n", test.opCode, test.clientVersion, &test);
	printf("RST: opCode: %d clientVersion: %d struct_addr: %p\n", temp.opCode, temp.clientVersion, &temp);

	system("pause");
}