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
	@file garbageCollector.c
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief 파일서버 가비지 콜렉터 관련 함수 모음
*/

#include "FileServer.h"

/**
	@fn unsigned int WINAPI waitingListGC()
	@brief 대기열 가비지 콜렉터, 인증시간이 초과한 대기열을 삭제함
	@author 멍멍아야옹해봐
*/
unsigned int WINAPI waitingListGC() {//미인증 장기 대기 방지 가비지 콜렉터
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

	return 0;
}