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
	@author 멍멍아야옹해봐
	@brief RelayServer 설정파일 관련 함수 모음
*/

#include "RelayServer.h"

/**
	@fn void checkRelayConfig()
	@brief 중계서버 설정파일 유무 검사
	@author 멍멍아야옹해봐
*/
void checkRelayConfig() {
	FILE *fp;

	if (fopen_s(&fp, "./RelayServerConfig.ini", "r")) {//파일서버 실정파일 읽을 수 있는지 검사
		printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: RelayServerConfig.ini File Error");
		system("pause");
		exit(1);//exit with error
	}
	fclose(fp);//설정파일 닫기
	return;
}

/**
	@fn void setErrorLevel()
	@brief 에러레벨 설정
	@author 멍멍아야옹해봐
*/
void setErrorLevel() {
	checkRelayConfig();

	errorLevel = GetPrivateProfileInt("DC_Common", "errorLevel", 0, "./RelayServerConfig.ini");
	return;
}

/**
@fn int readMaxConn()
@brief 최대 접속수 반환
@author 멍멍아야옹해봐
@return 최대 접속수
*/
int readMaxConn() {
	checkRelayConfig();
	return GetPrivateProfileInt("NetworkConfig", "max_conn", 100, "./RelayServerConfig.ini");
}

/**
@fn void readBindInfo(SOCKADDR_IN *servAddr)
@brief 서버 IP, 포트 설정 읽어옴
@author 멍멍아야옹해봐
@param *servAddr 서버 주소 정보 저장 구조체 포인터
*/
void readBindInfo(SOCKADDR_IN *servAddr) {
	checkRelayConfig();
	
	char serverIP[16] = { 0, };

	if (servAddr == NULL) return;
	memset(servAddr, 0, sizeof(SOCKADDR_IN));
	servAddr->sin_family = AF_INET;
	GetPrivateProfileString("Bind", "bind_addr", "0.0.0.0", serverIP, 16, "./RelayServerConfig.ini");
	servAddr->sin_port = htons((short)GetPrivateProfileInt("Bind", "bind_port", 15384, "./RelayServerConfig.ini"));

	if (!strcmp("any", serverIP))
		servAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	else
		inet_pton(AF_INET, serverIP, &servAddr->sin_addr.s_addr);

	return;
}

/**
	@fn void readMySQLConfig(MYSQL_SERVER *serverInfo)
	@brief MySQL 서버 설정 읽어옴
	@author 멍멍아야옹해봐
	@param *serverInfo MySQL 서버 정보 구조체 포인터
*/
void readMySQLConfig(MYSQL_SERVER *serverInfo) {
	checkRelayConfig();

	if (serverInfo == NULL) return;
	GetPrivateProfileString("DatabaseConfig", "mariadb_addr", "localhost", serverInfo->srvAddr, 255, "./RelayServerConfig.ini");
	serverInfo->srvPort = GetPrivateProfileInt("DatabaseConfig", "mariadb_port", 3306, "./RelayServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_db", "DogeCloud", serverInfo->dbase, 255, "./RelayServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_user", "root", serverInfo->user, 255, "./RelayServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_pwd", "", serverInfo->pass, 255, "./RelayServerConfig.ini");

	return;
}

/**
	@fn void readFileServerPath(SOCKADDR_IN *FileServAddr)
	@brief DogeCloud 파일 서버 경로 읽어오기
	@author 멍멍아야옹해봐
	@param *FileServAddr 중계 서버 연결용 주소 구조체 주소
*/
void readFileServerPath(SOCKADDR_IN *FileServAddr) {
	checkRelayConfig();
	if (FileServAddr == NULL) return;

	//init with zero
	memset(FileServAddr, 0, sizeof(*FileServAddr));

	//init addr
	char tmpAddr[16] = { 0, };
	FileServAddr->sin_family = AF_INET;
	GetPrivateProfileString("FileServer", "ip", "127.0.0.1", tmpAddr, 15, "./RelayServerConfig.ini");
	inet_pton(AF_INET, tmpAddr, &FileServAddr->sin_addr.s_addr);

	//init port
	FileServAddr->sin_port = htons(GetPrivateProfileInt("FileServer", "port", 15332, "./RelayServerConfig.ini"));
	return;
}