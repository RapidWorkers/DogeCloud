#include "FileServer.h"

void waitingListGC() {//������ ��� ��� ���� ������ �ݷ���
	int cleanCount;
	while (1) {
		cleanCount = 0;
		WaitForSingleObject(hMutex, INFINITE);
		for (int i = 0; i < authWaitCount; i++) {
			if (authWaitList[i].authWaitTime + 60 < (unsigned int)time(NULL)) {//60�ʰ� �α��� ���� ������
				memcpy(&authWaitList[i], &authWaitList[i + 1], sizeof(DC_AUTHWAIT_LIST)); //���� ��⿭�� ��ȿȭ�Ѵ�
				authWaitCount--;
				i--;//�Ѱ��� ���� �����Ǿ����Ƿ� i��°�� ���ؼ� �ݺ��ؾ���
				cleanCount++;
			}
		}
		ReleaseMutex(hMutex);
		printDebugMsg(DC_INFO, DC_ERRORLEVEL, "GC: Cleaned %d waiting List", cleanCount);
		Sleep(10000); //���� �۾����� 10�� ���
	}
}