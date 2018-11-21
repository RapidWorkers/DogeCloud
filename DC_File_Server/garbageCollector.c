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
	@author �۸۾ƾ߿��غ�
	@brief ���ϼ��� ������ �ݷ��� ���� �Լ� ����
*/

#include "FileServer.h"

/**
	@fn unsigned int WINAPI waitingListGC()
	@brief ��⿭ ������ �ݷ���, �����ð��� �ʰ��� ��⿭�� ������
	@author �۸۾ƾ߿��غ�
*/
unsigned int WINAPI waitingListGC() {//������ ��� ��� ���� ������ �ݷ���
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

	return 0;
}