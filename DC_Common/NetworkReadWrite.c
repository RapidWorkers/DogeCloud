#define DC_BUILD_DLL

#include "DC_Common.h"

#ifdef __cplusplus //C++ �����Ϸ��� �� �ڵ带 ������ �ϴ��� �˻�
extern "C" {
#endif

	/**
		@fn bool recvRaw(SOCKET socket, char* buffer, int recvByte, int flags)
		@brief ������ ũ�Ⱑ ���ŵ� �� ���� ����
		@author �۸۾ƾ߿��غ�
		@param socket ����
		@param buffer ��Ŷ ����
		@param recvByte ��Ŷ ������
		@param flags ���� �÷���
		@return 0 = ����, 1 = ����
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
		@brief ������ ũ�Ⱑ ���۵� �� ���� ����
		@author �۸۾ƾ߿��غ�
		@param socket ����
		@param buffer ��Ŷ ����
		@param sendByte ��Ŷ ������
		@param flags ���� �÷���
		@return 0 = ����, 1 = ����
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

#ifdef __cplusplus
}
#endif