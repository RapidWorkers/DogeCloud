#define DC_BUILD_DLL

#include "DC_Common.h"

//external library
#include "sha256.h"
#include "lea.h"
#include "duthomhas\csprng.h"
#pragma comment(lib, "dllLEA.lib")

#ifdef __cplusplus //check if cpp compiler compile this code.
extern "C" {
#endif

	DLL void printDebugMsg(int targetErrorLevel, int currentErrorLevel, const char* format, ...) {
		char buf[512] = { 0, };

		if (currentErrorLevel > 3) currentErrorLevel = 3; //만약에 최대값을 넘어갔다면 3으로 설정
		if (targetErrorLevel < currentErrorLevel) return; //유저가 지정한 레벨 이상의 것만 출력

		switch (targetErrorLevel) {
		case 0:
			strcpy_s(buf, 512, "[DEBUG]: ");
			break;
		case 1:
			strcpy_s(buf, 512, "[INFO]: ");
			break;
		case 2:
			strcpy_s(buf, 512, "[WARN]: ");
			break;
		case 3:
			strcpy_s(buf, 512, "[ERROR]: ");
			break;
		}

		va_list ap;
		va_start(ap, format);
		vsprintf_s(buf + strlen(buf), 512 - strlen(buf), format, ap);
		va_end(ap);

		puts(buf);//에러 정보 표시
		return;
	}

	DLL void doProgress(int step, int total)
	{
	}

	DLL void SHA256_Text(const char* text, char* buf) {
		if (text == NULL || buf == NULL) return;
		SHA256_CTX hSHA256;
		sha256_init(&hSHA256);
		sha256_update(&hSHA256, text, strlen(text));
		sha256_final(&hSHA256, buf);
		return;
	}

	DLL void GenerateCSPRNG(unsigned char *buffer, int numSize) {
		if (buffer == NULL) return;
		CSPRNG rng = csprng_create();
		if (!rng) return; //do nothing on error
		csprng_get(rng, buffer, numSize);
		rng = csprng_destroy(rng);
		return;
	}

	DLL void GenerateSessionKey(char sessionKey[32]) {
		if (sessionKey == NULL) return;
		unsigned char buffer[128] = { 0, };
		GenerateCSPRNG(buffer, 127);
		SHA256_Text(buffer, sessionKey);
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

	DLL void printLicense() {
		printf_s("이 프로그램에 포함된 라이브러리 및 소스 코드에 대한 라이센스는 \nhttps://github.com/RapidWorkers/DogeCloud 에서 확인하십시오.");
		printf_s("\
			Copyright (C) 2018 S.H.Kim (soohyunkim@kw.ac.kr)\
			Copyright(C) 2018 K.J Choi(chlrhkdwls99@naver.com)\
			\
			This program is free software; you can redistribute it and/or\
			modify it under the terms of the GNU General Public License\
			as published by the Free Software Foundation; either version 2\
			of the License, or (at your option) any later version.\
			\
			This program is distributed in the hope that it will be useful,\
			but WITHOUT ANY WARRANTY; without even the implied warranty of\
			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the\
			GNU General Public License for more details.\
			\
			You should have received a copy of the GNU General Public License\
			along with this program; if not, write to the Free Software\
			Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110 - 1301, USA.");
	}

#ifdef __cplusplus
}
#endif