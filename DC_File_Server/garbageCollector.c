#include "FileServer.h"

void waitingListGC() {//미인증 장기 대기 방지 가비지 콜렉터
	int cleanCount;
	while (1) {
		cleanCount = 0;
		WaitForSingleObject(hMutex, INFINITE);
		for (int i = 0; i < authWaitCount; i++) {
			if (authWaitList[i].authWaitTime + 60 < (unsigned int)time(NULL)) {//60초간 로그인 하지 않으면
				memcpy(&authWaitList[i], &authWaitList[i + 1], sizeof(DC_AUTHWAIT_LIST)); //인증 대기열을 무효화한다
				authWaitCount--;
				i--;//한개의 값이 삭제되었으므로 i번째에 대해서 반복해야함
				cleanCount++;
			}
		}
		ReleaseMutex(hMutex);
		printDebugMsg(DC_INFO, DC_ERRORLEVEL, "GC: Cleaned %d waiting List", cleanCount);
		Sleep(10000); //다음 작업까지 10초 대기
	}
}