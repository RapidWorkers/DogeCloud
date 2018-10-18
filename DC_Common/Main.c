#include <stdio.h>
#include <string.h>

#include "sqlite3.h"
#include "sha256.h"

#include "SessionPacket.h"

#include "lea.h"
#pragma comment(lib, "dllLEA.lib")

#define DLL _declspec(dllexport)

#ifdef __cplusplus //check if cpp compiler compile this code.
extern "C" {
#endif
DLL void myFunc() {
	printf("Hi!\n");
	}

DLL void testLEA() {
	unsigned char testCase[] = "Hello World!";
	unsigned char decOutput[100];
	unsigned char encOutput[100];
	LEA_KEY newKey;
	lea_set_key(&newKey, "aaabbbcccdddeeef", 16);
	lea_ctr_enc(encOutput, testCase, sizeof(testCase), "aaabbbcccdddeee", &newKey);//IV will be Saved at end of file, 32bytes
	lea_ctr_enc(decOutput, encOutput, sizeof(testCase), "aaabbbcccdddeee", &newKey);
	printf("%s\n", decOutput);
	}

DLL void testPacketStructure() {
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
	}

DLL void testSHA256() {
	unsigned char testCase[] = "Hello World!";
	unsigned char hashed[32];
	printf("ORG: %s\n", testCase);
	SHA256_CTX testSHA;
	sha256_init(&testSHA);
	sha256_update(&testSHA, testCase, sizeof(testCase)-1);
	sha256_final(&testSHA, hashed);

	printf("Hashed: \n");
	for (int i = 0; i < 32; i++)
	{
		printf("%02x", hashed[i]);
	}
	printf("\n");
	
	unsigned char compareTarget[32] = { 0x7F, 0x83, 0xB1, 0x65 ,0x7F, 0xF1, 0xFC, 0x53, 0xB9, 0x2D, 0xC1, 0x81, 0x48, 0xA1, 0xD6, 0x5D, 0xFC, 0x2D, 0x4B, 0x1F, 0xA3, 0xD6, 0x77, 0x28, 0x4A, 0xDD, 0xD2, 0x00, 0x12, 0x6D, 0x90, 0x69 };

	printf("Compare: \n");
	for (int i = 0; i < 32; i++)
	{
		printf("%02x", compareTarget[i]);
	}
	printf("\n");

	int flag = 1;

	for (int i = 0; i < 32; i++) {
		if (compareTarget[i] != hashed[i]) {
			flag = 0;
			break;
		}
	}

	if (flag)
		printf("Hash Match!\n");
	else
		printf("Hash Mismatch..");

	}
#ifdef __cplusplus
}
#endif