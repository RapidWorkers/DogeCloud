#define DC_BUILD_DLL

#include "DC_Common.h"

//�ܺ� ���̺귯�� �ε�
#include "sha256.h"
#include "lea.h"
#include "duthomhas\csprng.h"
#pragma comment(lib, "dllLEA.lib")

#ifdef __cplusplus //CPP ȣȯ���� ����
extern "C" {
#endif
	/**
		@fn void printDebugMsg(int targetErrorLevel, int currentErrorLevel, const char* format, ...)
		@brief ����� �޽��� ǥ��
		@author �۸۾ƾ߿��غ�
		@param targetErrorLevel ���� ����
		@param currentErrorLevel ������ ���� ����
		@param *format ���� ���ڿ�
		@param ... ��Ÿ �Ķ����
	*/
	DLL void printDebugMsg(int targetErrorLevel, int currentErrorLevel, const char* format, ...) {
		char buf[512] = { 0, };

		if (currentErrorLevel > 3) currentErrorLevel = 3; //���࿡ �ִ밪�� �Ѿ�ٸ� 3���� ����
		if (targetErrorLevel < currentErrorLevel) return; //������ ������ ���� �̻��� �͸� ���

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

		puts(buf);//���� ���� ǥ��
		return;
	}

	/**
		@fn void updateProgress(int current, int total)
		@brief ���α׷��� �� ����/������Ʈ
		@author �۸۾ƾ߿��غ�
		@param current ���� ��ġ
		@param total ��ü ��ġ
	*/
	DLL void updateProgress(int current, int total)
	{	
		int length = 30;//���α׷��� �� ����
		double tickrate = (double)100 / length;//�� %�� �ϳ��� �ø�����
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
		@brief ���ڿ��� SHA256 ���ϱ�
		@author �۸۾ƾ߿��غ�
		@param *text ���� ���ڿ�
		@param *buf ������ ����(32����Ʈ)
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
		@brief ��ȣ�������� ������ ���� ����
		@author �۸۾ƾ߿��غ�
		@param *buffer ������ ����
		@param numSize ������ ũ��
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
		@brief ����Ű ����
		@author �۸۾ƾ߿��غ�
		@param *sessionKey ����Ű ������ �迭(32����Ʈ)
	*/
	DLL void GenerateSessionKey(char *sessionKey) {
		if (sessionKey == NULL) return;
		unsigned char buffer[128] = { 0, };
		GenerateCSPRNG(buffer, 127);
		SHA256_Text(buffer, sessionKey);
	}

	/**
		@fn void printProgramInfo()
		@brief DogeCloud ���� ǥ��
		@author �۸۾ƾ߿��غ�
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
		@brief DogeCloud ���̼��� ǥ��
		@author �۸۾ƾ߿��غ�
	*/
	DLL void printLicense() {
		puts("�� ���α׷��� ���Ե� ���̺귯�� �� �ҽ� �ڵ忡 ���� ���̼����� \nhttps://github.com/RapidWorkers/DogeCloud ���� Ȯ���Ͻʽÿ�.");
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