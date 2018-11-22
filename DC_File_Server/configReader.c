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
	@author ¸Û¸Û¾Æ¾ß¿ËÇØºÁ
	@brief FileServer ¼³Á¤ ÆÄÀÏ °ü·Ã ÇÔ¼ö ¸ðÀ½
*/

#include "FileServer.h"

/**
	@fn void checkFileConfig()
	@brief ÆÄÀÏ¼­¹ö ¼³Á¤ÆÄÀÏ À¯¹« °Ë»ç
	@author ¸Û¸Û¾Æ¾ß¿ËÇØºÁ
*/
void checkFileConfig() {
	FILE *fp;

	if (fopen_s(&fp, "./FileServerConfig.ini", "r")) {//¿­¾îº¸°í ¾øÀ¸¸é ¿¡·¯
		printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: FileServerConfig.ini File Error");
		system("pause");
		exit(1);//exit with error
	}
	fclose(fp);//ÆÄÀÏÀ» ´Ý´Â´Ù
	return;
}

/**
	@fn void setErrorLevel()
	@brief ¿¡·¯·¹º§ ¼³Á¤
	@author ¸Û¸Û¾Æ¾ß¿ËÇØºÁ
*/
void setErrorLevel() {
	checkFileConfig();

	errorLevel = GetPrivateProfileInt("DC_Common", "errorLevel", 0, "./FileServerConfig.ini");
	return;
}

/**
@fn int readMaxConn()
@brief ÃÖ´ë Á¢¼Ó¼ö ¹ÝÈ¯
@author ¸Û¸Û¾Æ¾ß¿ËÇØºÁ
@return ÃÖ´ë Á¢¼Ó¼ö
*/
int readMaxConn() {
	checkFileConfig();
	return GetPrivateProfileInt("NetworkConfig", "max_conn", 100, "./FileServerConfig.ini");
}

/**
@fn void readBindInfo(SOCKADDR_IN *servAddr)
@brief ¼­¹ö IP, Æ÷Æ® ¼³Á¤ ÀÐ¾î¿È
@author ¸Û¸Û¾Æ¾ß¿ËÇØºÁ
@param *servAddr ¼­¹ö ÁÖ¼Ò Á¤º¸ ÀúÀå ±¸Á¶Ã¼ Æ÷ÀÎÅÍ
*/
void readBindInfo(SOCKADDR_IN *servAddr) {
	checkFileConfig();

	char serverIP[16] = { 0, };

	if (servAddr == NULL) return;
	memset(servAddr, 0, sizeof(SOCKADDR_IN));
	servAddr->sin_family = AF_INET;
	GetPrivateProfileString("Bind", "bind_addr", "0.0.0.0", serverIP, 16, "./FileServerConfig.ini");
	servAddr->sin_port = htons((short)GetPrivateProfileInt("Bind", "bind_port", 15332, "./FileServerConfig.ini"));

	if (!strcmp("any", serverIP))
		servAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	else
		inet_pton(AF_INET, serverIP, &servAddr->sin_addr.s_addr);

	return;
}

/**
	@fn void readMySQLConfig(MYSQL_SERVER *serverInfo)
	@brief MySQL ¼­¹ö ¼³Á¤ ÀÐ¾î¿È
	@author ¸Û¸Û¾Æ¾ß¿ËÇØºÁ
	@param *serverInfo MySQL ¼­¹ö Á¤º¸ ±¸Á¶Ã¼ Æ÷ÀÎÅÍ
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