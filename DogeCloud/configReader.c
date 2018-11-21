/*
Copyright (C) 2018 S.H.Kim (soohyunkim@kw.ac.kr)
Copyright (C) 2018 K.J Choi (chlrhkdwls99@naver.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/**
	@file configReader.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief DogeCloud �������� ���� �Լ� ����
*/

#include "DogeCloud.h"

/**
	@fn void checkDogeConfig()
	@brief DogeCloud �߰� ���� �������� ���� Ȯ��
	@author �۸۾ƾ߿��غ�
*/
void checkDogeConfig() {
	FILE *fp;

	if (fopen_s(&fp, "./DogeConfig.ini", "r")) {//���� �ִ��� �˻�
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "DogeConfig ������ �����ϴ�.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "������ ���� �� �ּ���.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);//���α׷� �����ڵ� �߻� �� ����
	}
	fclose(fp);//���� �ݱ�
	return;
}

/**
	@fn void readRelayServerPath(SOCKADDR_IN *RelayServAddr)
	@brief DogeCloud �߰� ���� �������� ���� Ȯ��
	@author �۸۾ƾ߿��غ�
	@param *RelayServAddr �߰� ���� ����� �ּ� ����ü �ּ�
*/
void readRelayServerPath(SOCKADDR_IN *RelayServAddr) {
	checkDogeConfig();//�������� Ȯ��
	if (RelayServAddr == NULL) return;//�ּ� ������ ����

	//�޾ƿ� ����ü 0���� �ʱ�ȭ
	memset(RelayServAddr, 0, sizeof(SOCKADDR_IN));

	//�ּ� ����
	char tmpAddr[16] = { 0, };//�ּ� ����� �迭
	RelayServAddr->sin_family = AF_INET;
	GetPrivateProfileString("RelayServer", "ip", "127.0.0.1", tmpAddr, 15, "./DogeConfig.ini");//ini���� �о��
	inet_pton(AF_INET, tmpAddr, &RelayServAddr->sin_addr.s_addr);//�ּ� ��ȯ �� ����

	//��Ʈ ����
	RelayServAddr->sin_port = htons(GetPrivateProfileInt("RelayServer", "port", 15384, "./DogeConfig.ini"));
	return;
}