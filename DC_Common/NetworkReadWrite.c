#define DC_BUILD_DLL

#include "DC_Common.h"

//to prevent unexpected packet loss
//we have to implement new network functioin

/**
	@fn bool recvRaw(SOCKET socket, char* buffer, int sendByte, int flags)
	@brief 지정할 크기가 수신될 때 까지 수신
	@author 멍멍아야옹해봐
	@param socket 소켓
	@param packetBuffer 패킷 버퍼
	@param packetSize 패킷 사이즈
	@param flag 소켓 플래그
	@return 0 = 실패, 1 = 성공
*/
bool recvRaw(SOCKET socket, char* buffer, int recvByte, int flags) {
	int left = recvByte;
	do {
		int recvLen = recv(socket, buffer, left, flags);
		if (recvLen <= 0) return 0; //read nothing at socket error
		left -= recvLen;
	} while (left);
	return 1;
}

/**
	@fn bool sendRaw(SOCKET socket, char* buffer, int sendByte, int flags)
	@brief 지정할 크기가 전송될 때 까지 전송
	@author 멍멍아야옹해봐
	@param socket 소켓
	@param packetBuffer 패킷 버퍼
	@param packetSize 패킷 사이즈
	@param flag 소켓 플래그
	@return 0 = 실패, 1 = 성공
*/
bool sendRaw(SOCKET socket, char* buffer, int sendByte, int flags) {
	int left = sendByte;
	do {
		int sendLen = send(socket, buffer, left, flags);
		if (sendLen <= 0) return 0; //send nothing at socket error
		left -= sendLen;
	} while (left);
	return 1;
}

//bool checkStatus(SOCKET socket) {
//
//}