#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sqlite3.h"
#include "sha256.h"

#include "SessionPacket.h"

#include "lea.h"
#pragma comment(lib, "dllLEA.lib")

#define DLL _declspec(dllexport)

#ifdef __cplusplus //check if cpp compiler compile this code.
extern "C" {
#endif

	DLL void printDebugMsg(int targetErrorLevel, int currentErrorLevel, char* buffer) {
		if (currentErrorLevel > 3) currentErrorLevel = 3; //Always display error
		if (targetErrorLevel < currentErrorLevel) return; //only display error that exceeds user setting
		switch (targetErrorLevel) {
		case 0:
			puts("[DEBUG]: ");
			break;
		case 1:
			puts("[INFO]: ");
			break;
		case 2:
			puts("[WARN]: ");
			break;
		case 3:
			puts("[ERROR]: ");
			break;
		}
		puts(buffer);//display error info
		return;
	}

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

		test.opCode = 102;
		test.clientVersion = 123456789;

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
	}

	DLL void testSHA256() {
		unsigned char testCase[] = "Hello World!";
		unsigned char hashed[32];
		printf("ORG: %s\n", testCase);
		SHA256_CTX testSHA;
		sha256_init(&testSHA);
		sha256_update(&testSHA, testCase, sizeof(testCase) - 1);
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