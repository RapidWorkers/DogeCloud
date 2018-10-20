#include "DC_Common.h"

//external library
#include "sqlite3.h"
#include "sha256.h"
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
			printf_s("[DEBUG]: ");
			break;
		case 1:
			printf_s("[INFO]: ");
			break;
		case 2:
			printf_s("[WARN]: ");
			break;
		case 3:
			printf_s("[ERROR]: ");
			break;
		}
		printf_s("%s\n", buffer);//display error info
		return;
	}

	DLL void SHA256_Text(const char* text, char* buf) {
		SHA256_CTX hSHA256;
		sha256_init(&hSHA256);
		sha256_update(&hSHA256, text, strlen(text));
		sha256_final(&hSHA256, buf);
		return;
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