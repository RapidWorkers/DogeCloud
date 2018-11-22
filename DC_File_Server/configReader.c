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
	@brief FileServer ���� ���� ���� �Լ� ����
*/

#include "FileServer.h"

/**
	@fn void checkFileConfig()
	@brief ���ϼ��� �������� ���� �˻�
	@author �۸۾ƾ߿��غ�
*/
void checkFileConfig() {
	FILE *fp;

	if (fopen_s(&fp, "./FileServerConfig.ini", "r")) {//����� ������ ����
		printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: FileServerConfig.ini File Error");
		system("pause");
		exit(1);//exit with error
	}
	fclose(fp);//������ �ݴ´�
	return;
}

/**
	@fn void setErrorLevel()
	@brief �������� ����
	@author �۸۾ƾ߿��غ�
*/
void setErrorLevel() {
	checkFileConfig();

	errorLevel = GetPrivateProfileInt("DC_Common", "errorLevel", 0, "./FileServerConfig.ini");
	return;
}

/**
	@fn void readMySQLConfig(MYSQL_SERVER *serverInfo)
	@brief MySQL ���� ���� �о��
	@author �۸۾ƾ߿��غ�
	@param *serverInfo MySQL ���� ���� ����ü ������
*/
void readMySQLConfig(MYSQL_SERVER *serverInfo) {
	checkFileConfig();

	if (serverInfo == NULL) return;
	GetPrivateProfileString("DatabaseConfig", "mariadb_addr", "localhost", serverInfo->srvAddr, 255, "./FileServerConfig.ini");
	serverInfo->srvPort = GetPrivateProfileInt("DatabaseConfig", "mariadb_port", 3306, "./FileServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_db", "DogeCloud", serverInfo->dbase, 255, "./FileServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_user", "root", serverInfo->user, 255, "./FileServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_pwd", "", serverInfo->pass, 255, "./FileServerConfig.ini");

	return;
}