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
	DLL bool recvRaw(SOCKET socket, char* buffer, int recvByte, int flags) {
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
	DLL bool sendRaw(SOCKET socket, char* buffer, int sendByte, int flags) {
		int left = sendByte;
		do {
			int sendLen = send(socket, buffer, left, flags);
			if (sendLen <= 0) return 0; //send nothing at socket error
			left -= sendLen;
		} while (left);
		return 1;
	}

	/**
	@fn bool uploadFile(SOCKET socket, FILE *file)
	@brief ������ ���ε��Ѵ�.
	@author �۸۾ƾ߿��غ�
	@param socket ����
	@param *file ���� ������
	@return 0 = ����, 1 = ����
	*/
	DLL bool uploadFile(SOCKET socket, FILE *file) {
		unsigned long fileSize = 0;
		unsigned long toRead = 0;
		fseek(file, 0, SEEK_END);
		fileSize = ftell(file);

		fileSize = htonl(fileSize);
		if (!sendRaw(socket, (char*)&fileSize, 4, 0)) {
			return 0;
		}
		fileSize = ntohl(fileSize);

		fseek(file, 0, SEEK_SET);

		unsigned long left = fileSize;

		unsigned char dataBuffer[NETWORK_CACHE_SIZE];
		while (left) {
			if (left < NETWORK_CACHE_SIZE)
				toRead = left;
			else
				toRead = NETWORK_CACHE_SIZE;

			fread(dataBuffer, toRead, 1, file);
			if (!sendRaw(socket, dataBuffer, toRead, 0)) {
				rewind(file);
				return 0;
			}
			left -= toRead;
		}
		rewind(file);
		return 1;
	}

	/**
	@fn bool downloadFile(SOCKET socket, FILE *file)
	@brief ������ �ٿ�ε��Ѵ�.
	@author �۸۾ƾ߿��غ�
	@param socket ����
	@param *file ���� ������
	@return 0 = ����, 1 = ����
	*/
	DLL bool downloadFile(SOCKET socket, FILE *file) {
		unsigned long fileSize = 0;
		unsigned long toWrite = 0;

		if (!recvRaw(socket, (char*)&fileSize, 4, 0)) {
			return 0;
		}

		fileSize = ntohl(fileSize);

		unsigned long left = fileSize;

		unsigned char dataBuffer[NETWORK_CACHE_SIZE];
		while (left) {
			if (left < NETWORK_CACHE_SIZE)
				toWrite = left;
			else
				toWrite = NETWORK_CACHE_SIZE;

			if (!recvRaw(socket, dataBuffer, toWrite, 0)) {
				rewind(file);
				return 0;
			}
			fwrite(dataBuffer, toWrite, 1, file);
			left -= toWrite;
		}
		rewind(file);
		return 1;
	}

	/**
	@fn bool uploadFileProgress(SOCKET socket, FILE *file)
	@brief ������ ���ε��Ѵ�.(���α׷��� �� ����)
	@author �۸۾ƾ߿��غ�
	@param socket ����
	@param *file ���� ������
	@return 0 = ����, 1 = ����
	*/
	DLL bool uploadFileProgress(SOCKET socket, FILE *file) {
		unsigned long fileSize = 0;
		unsigned long toRead = 0;
		fseek(file, 0, SEEK_END);
		fileSize = ftell(file);

		fileSize = htonl(fileSize);
		if (!sendRaw(socket, (char*)&fileSize, 4, 0)) {
			return 0;
		}
		fileSize = ntohl(fileSize);

		fseek(file, 0, SEEK_SET);

		unsigned long left = fileSize;

		unsigned char dataBuffer[NETWORK_CACHE_SIZE];
		while (left) {
			if (left < NETWORK_CACHE_SIZE)
				toRead = left;
			else
				toRead = NETWORK_CACHE_SIZE;

			fread(dataBuffer, toRead, 1, file);
			if (!sendRaw(socket, dataBuffer, toRead, 0)) {
				rewind(file);
				return 0;
			}
			left -= toRead;
			updateProgress(fileSize - left, fileSize);
		}

		puts("");
		rewind(file);
		return 1;
	}

	/**
	@fn bool downloadFileProgress(SOCKET socket, FILE *file)
	@brief ������ �ٿ�ε��Ѵ�.(���α׷��� �� ����)
	@author �۸۾ƾ߿��غ�
	@param socket ����
	@param *file ���� ������
	@return 0 = ����, 1 = ����
	*/
	DLL bool downloadFileProgress(SOCKET socket, FILE *file) {
		unsigned long fileSize = 0;
		unsigned long toWrite = 0;

		if (!recvRaw(socket, (char*)&fileSize, 4, 0)) {
			return 0;
		}

		fileSize = ntohl(fileSize);

		unsigned long left = fileSize;

		unsigned char dataBuffer[NETWORK_CACHE_SIZE];
		while (left) {
			if (left < NETWORK_CACHE_SIZE)
				toWrite = left;
			else
				toWrite = NETWORK_CACHE_SIZE;

			if (!recvRaw(socket, dataBuffer, toWrite, 0)) {
				rewind(file);
				return 0;
			}
			fwrite(dataBuffer, toWrite, 1, file);
			left -= toWrite;
			updateProgress(fileSize - left, fileSize);
		}

		puts("");
		rewind(file);
		return 1;
	}

#ifdef __cplusplus
}
#endif