#define DC_BUILD_DLL

#include "DC_Common.h"

//외부 라이브러리 로드
#include "sha256.h"
#include "lea.h"
#include "duthomhas\csprng.h"
#pragma comment(lib, "dllLEA.lib")

#ifdef __cplusplus //CPP 호환성을 위해
extern "C" {
#endif
	/**
		@fn void printDebugMsg(int targetErrorLevel, int currentErrorLevel, const char* format, ...)
		@brief 디버그 메시지 표시
		@author 멍멍아야옹해봐
		@param targetErrorLevel 오류 수준
		@param currentErrorLevel 설정된 오류 수준
		@param *format 서식 문자열
		@param ... 기타 파라미터
	*/
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

	/**
		@fn void updateProgress(int current, int total)
		@brief 프로그레스 바 생성/업데이트
		@author 멍멍아야옹해봐
		@param current 현재 수치
		@param total 전체 수치
	*/
	DLL void updateProgress(int current, int total)
	{	
		int length = 30;//프로그레스 바 길이
		double tickrate = (double)100 / length;//몇 %당 하나씩 늘릴건지
		double percent;

		printf_s("\r%d / %d [", current, total);
		percent = (double)current / total * 100;

		int count = percent / tickrate;

		for (int i = 0; i < length; i++) {
			if (count > i)
				printf_s("=");
			else
				printf_s(" ");
		}
		printf_s("] %.1lf %%", percent);
		return;
	}

	/**
		@fn void SHA256_Text(const char* text, char* buf)
		@brief 문자열의 SHA256 구하기
		@author 멍멍아야옹해봐
		@param *text 구할 문자열
		@param *buf 저장할 공간(32바이트)
	*/
	DLL void SHA256_Text(const char* text, char* buf) {
		if (text == NULL || buf == NULL) return;
		SHA256_CTX hSHA256;
		sha256_init(&hSHA256);
		sha256_update(&hSHA256, text, strlen(text));
		sha256_final(&hSHA256, buf);
		return;
	}

	/**
		@fn void GenerateCSPRNG(unsigned char *buffer, int numSize)
		@brief 암호학적으로 안전한 난수 생성
		@author 멍멍아야옹해봐
		@param *buffer 저장할 공간
		@param numSize 생성할 크기
	*/
	DLL void GenerateCSPRNG(unsigned char *buffer, int numSize) {
		if (buffer == NULL) return;
		CSPRNG rng = csprng_create();
		if (!rng) return; //do nothing on error
		csprng_get(rng, buffer, numSize);
		rng = csprng_destroy(rng);
		return;
	}

	/**
		@fn void GenerateSessionKey(char *sessionKey)
		@brief 세션키 생성
		@author 멍멍아야옹해봐
		@param *sessionKey 세션키 저장할 배열(32바이트)
	*/
	DLL void GenerateSessionKey(char *sessionKey) {
		if (sessionKey == NULL) return;
		unsigned char buffer[128] = { 0, };
		GenerateCSPRNG(buffer, 127);
		SHA256_Text(buffer, sessionKey);
	}

	/**
		@fn void printProgramInfo()
		@brief DogeCloud 정보 표시
		@author 멍멍아야옹해봐
	*/
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
	
	/**
		@fn void printLicense()
		@brief DogeCloud 라이선스 표시
		@author 멍멍아야옹해봐
	*/
	DLL void printLicense() {
		puts("이 프로그램에 포함된 라이브러리 및 소스 코드에 대한 라이센스는 \nhttps://github.com/RapidWorkers/DogeCloud 에서 확인하십시오.");
		puts("\
Copyright (C) 2018 S.H.Kim (soohyunkim@kw.ac.kr)\n\
Copyright(C) 2018 K.J Choi(chlrhkdwls99@naver.com)\n\
\n\
This program is free software; you can redistribute it and/or\n\
modify it under the terms of the GNU General Public License\n\
as published by the Free Software Foundation; either version 2\n\
of the License, or (at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program; if not, write to the Free Software\n\
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110 - 1301, USA.");
	}

#ifdef __cplusplus
}
#endif