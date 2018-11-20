#include "DogeCloud.h"

void manageContacts(SOCKET hSocket) {
	return;
}

void manageMemo(SOCKET hSocket) {
	return;
}

void manageFile(SOCKET hSocket) {
	cs_FileSrvConReq FileSrvConReq;
	sc_FileSrvConResp FileSrvConResp;
	memset(&FileSrvConReq, 0, sizeof(cs_FileSrvConReq));
	memset(&FileSrvConResp, 0, sizeof(sc_FileSrvConResp));

	FileSrvConReq.Data.opCode = htonl(OP_CS_FILESRVCONNREQ);
	FileSrvConReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 4);

	sendData(hSocket, FileSrvConReq.buf, sizeof(cs_FileSrvConReq), 0);
	recvData(hSocket, FileSrvConResp.buf, sizeof(sc_FileSrvConResp), 0);

	//convert number to host specific
	FileSrvConResp.Data.opCode = ntohl(FileSrvConResp.Data.opCode);
	FileSrvConResp.Data.dataLen = ntohl(FileSrvConResp.Data.dataLen);
	
	if (!FileSrvConResp.Data.statusCode) {
		puts("���ϼ��� ������ ������ �� �����ϴ�!");
		puts("���� ������ ������ �ְų� ��� ó���� �����Ǵ� ���� �� �ֽ��ϴ�. ���߿� �ٽ� �õ����ּ���.");
		system("pause");
		return;
	}

	openFileServer(FileSrvConResp.Data.fileSrvAddr, FileSrvConResp.Data.fileSrvPort, FileSrvConResp.Data.authKey);
	return;
}