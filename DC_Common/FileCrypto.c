#include "DC_Common.h"
#include "sha256.h"
#include "lea.h"
#include "duthomhas\csprng.h"
#pragma comment(lib, "dllLEA.lib")

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

DLL void generateNonce(char* nonceBuffer, int nonceSize) {
	CSPRNG rng = csprng_create();
	if (!rng) return; //do nothing on error
	csprng_get(rng, nonceBuffer, nonceSize);
	rng = csprng_destroy(rng);
	return;
};

DLL void encryptFileLEA(FILE *file) {
//will be implemented
}

DLL void decryptFileLEA(FILE *file) {
//will be implemented
}

DLL void getFileHash(FILE *file) {
//will be implemented
}