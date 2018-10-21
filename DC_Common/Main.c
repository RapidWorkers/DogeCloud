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

	DLL void printProgramInfo() {
		unsigned char logo_txt[] = {
			0x20, 0x20, 0x5f, 0x5f, 0x5f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5f, 0x5f,
			0x5f, 0x20, 0x5f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x5f, 0x20, 0x0d, 0x0a, 0x20, 0x7c, 0x20, 0x20,
			0x20, 0x5c, 0x20, 0x5f, 0x5f, 0x5f, 0x20, 0x20, 0x5f, 0x5f, 0x20, 0x5f,
			0x20, 0x5f, 0x5f, 0x5f, 0x20, 0x2f, 0x20, 0x5f, 0x5f, 0x7c, 0x20, 0x7c,
			0x5f, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x20, 0x5f, 0x20, 0x5f, 0x5f, 0x7c,
			0x20, 0x7c, 0x0d, 0x0a, 0x20, 0x7c, 0x20, 0x7c, 0x29, 0x20, 0x2f, 0x20,
			0x5f, 0x20, 0x5c, 0x2f, 0x20, 0x5f, 0x60, 0x20, 0x2f, 0x20, 0x2d, 0x5f,
			0x29, 0x20, 0x28, 0x5f, 0x5f, 0x7c, 0x20, 0x2f, 0x20, 0x5f, 0x20, 0x5c,
			0x20, 0x7c, 0x7c, 0x20, 0x2f, 0x20, 0x5f, 0x60, 0x20, 0x7c, 0x0d, 0x0a,
			0x20, 0x7c, 0x5f, 0x5f, 0x5f, 0x2f, 0x5c, 0x5f, 0x5f, 0x5f, 0x2f, 0x5c,
			0x5f, 0x5f, 0x2c, 0x20, 0x5c, 0x5f, 0x5f, 0x5f, 0x7c, 0x5c, 0x5f, 0x5f,
			0x5f, 0x7c, 0x5f, 0x5c, 0x5f, 0x5f, 0x5f, 0x2f, 0x5c, 0x5f, 0x2c, 0x5f,
			0x5c, 0x5f, 0x5f, 0x2c, 0x5f, 0x7c, 0x0d, 0x0a, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7c, 0x5f, 0x5f, 0x5f, 0x2f,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x00
		};
		printf_s("%s\n", logo_txt);
		printf_s("=======================================\n");
		printf_s("DC_Common.dll built at %s %s\n", __DATE__, __TIME__);
		printf_s("\tby S.H Kim and K.J Choi at kw.ac.kr\n");
		printf_s("\tVersion: %d.%d (%s)\n", MAJOR_VERSION, MINOR_VERSION, VER_STATUS);
		printf_s("=======================================\n");
}
#ifdef __cplusplus
}
#endif