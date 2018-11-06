#include "DC_Common.h"
#include "sha256.h"
#include "lea.h"
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

DLL void encryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV) {// IV MUST BE PROVIDED & SHOULD BE 16 Byte, KEY Size: 32
	if (infile == NULL || outfile == NULL) return;
	fseek(infile, 0, SEEK_END);
	unsigned int left = ftell(infile);
	fseek(infile, 0, SEEK_SET);//return to start pos

	//copy iv to new var -> it'll be modified after encryption
	char ctr_counter[16];
	memcpy_s(ctr_counter,16, nonceIV, 16);

	//init lea
	LEA_KEY leaKey;
	lea_set_key(&leaKey, encKey, 32);

	//init read and encryption buffer
	char org_buffer[2048];
	char enc_buffer[2048];
	int toRead;

	while (left) {
		if (left < 2048)
			toRead = left;
		else
			toRead = 2048;

		fread(org_buffer, toRead, 1, infile); //read original data
		lea_ctr_enc(enc_buffer, org_buffer, toRead, ctr_counter, &leaKey);//encrypt it
		fwrite(enc_buffer, toRead, 1, outfile);//write to output

		left -= toRead;
	}
}

DLL void decryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV) {
	if (infile == NULL || outfile == NULL) return;
	fseek(infile, 0, SEEK_END);
	unsigned int left = ftell(infile);
	fseek(infile, 0, SEEK_SET);//return to start pos

	//copy iv to new var -> it'll be modified after encryption
	char ctr_counter[16];
	memcpy_s(ctr_counter, 16, nonceIV, 16);

	//init lea
	LEA_KEY leaKey;
	lea_set_key(&leaKey, encKey, 32);

	//init read and encryption buffer
	char org_buffer[2048];
	char dec_buffer[2048];
	int toRead;

	while (left) {
		if (left < 2048)
			toRead = left;
		else
			toRead = 2048;

		fread(org_buffer, toRead, 1, infile); //read original data
		lea_ctr_dec(dec_buffer, org_buffer, toRead, ctr_counter, &leaKey);//encrypt it
		fwrite(dec_buffer, toRead, 1, outfile);//write to output

		left -= toRead;
	}
}

DLL void getFileHash(FILE *file, char* result) {
	if (file == NULL) return;
	fseek(file, 0, SEEK_END);
	unsigned int left = ftell(file);
	fseek(file, 0, SEEK_SET);//return to start pos

	//init SHA256
	SHA256_CTX hSHA256;
	sha256_init(&hSHA256);
	int toRead;

	//init read Buffer
	char buffer[2048];

	while (left) {
		if (left < 2048)
			toRead = left;
		else
			toRead = 2048;

		fread(buffer, toRead, 1, file); //read original data
		sha256_update(&hSHA256, buffer, toRead);//calc sha256
		left -= toRead;
	}
	sha256_final(&hSHA256, result);
	return;
}