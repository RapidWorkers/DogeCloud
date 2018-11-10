#define DC_BUILD_DLL

#include "DC_Common.h"

//to prevent unexpected packet loss
//we have to implement new network functioin

bool recvRaw(SOCKET socket, char* buffer, int recvByte, int flags) {
	int left = recvByte;
	do {
		int recvLen = recv(socket, buffer, left, flags);
		if (recvLen <= 0) return 0; //read nothing at socket error
		left -= recvLen;
	} while (left);
	return 1;
}

bool sendRaw(SOCKET socket, char* buffer, int sendByte, int flags) {
	int left = sendByte;
	do {
		int sendLen = send(socket, buffer, left, flags);
		if (sendLen <= 0) return 0; //send nothing at socket error
		left -= sendLen;
	} while (left);
	return 1;
}