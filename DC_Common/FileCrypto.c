#define DC_BUILD_DLL

#include "DC_Common.h"
#include "sha256.h"
#include "lea.h"
#pragma comment(lib, "dllLEA.lib")

#ifdef __cplusplus //CPP 호환성을 위해
extern "C" {
#endif

	/**
		@fn void encryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV)
		@brief LEA 암호화 알고리즘으로 파일 암호화
		@author 멍멍아야옹해봐
		@param *infile 입력 파일
		@param *outfile 출력 파일
		@param *encKey 암호화 키(32바이트)
		@param *nonceIV 초기화 난수(16바이트)
	*/
	DLL void encryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV) {
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

		//초기 위치로 복귀
		fseek(infile, 0, SEEK_SET);
		fseek(outfile, 0, SEEK_SET);
		return;
	}

	/**
		@fn void decryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV)
		@brief LEA 암호화 알고리즘으로 파일 복호화
		@author 멍멍아야옹해봐
		@param *infile 입력 파일
		@param *outfile 출력 파일
		@param *encKey 암호화 키(32바이트)
		@param *nonceIV 초기화 난수(16바이트), 암호화 시와 동일 키 사용해야함
	*/
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

		//초기 위치로 복귀
		fseek(infile, 0, SEEK_SET);
		fseek(outfile, 0, SEEK_SET);
		return;
	}

	/**
		@fn void getFileHash(FILE *file, char* result)
		@brief 파일의 SHA256 해쉬 구함
		@author 멍멍아야옹해봐
		@param *file 파일 구조체 포인터
		@param *result 결과를 저장할 공간(32바이트)
	*/
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

		fseek(file, 0, SEEK_SET);//return to start pos
		return;
	}
#ifdef __cplusplus
}
#endif