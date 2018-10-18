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
	unsigned char hashed[64];
	printf("ORG: %s\n", testCase);
	SHA256_CTX testSHA;
	sha256_init(&testSHA);
	sha256_update(&testSHA, testCase, sizeof(testCase)-1);
	sha256_final(&testSHA, hashed);

	for (int i = 0; i < 32; i++)
	{
		printf("%02x", hashed[i]);
	}

	}
#ifdef __cplusplus
}
#endif