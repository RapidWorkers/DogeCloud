#include "DataManagePacket.h"
#include "SessionPacket.h"
#include "UserCommPacket.h"
#include "FileServerCommPacket.h"

#ifdef __cplusplus //check if cpp compiler compile this code.
extern "C" {
#endif
	_declspec(dllimport) void myFunc();//prototype for dll exported function, which used in other program to load function
	_declspec(dllimport) void testLEA();
	_declspec(dllimport) void testPacketStructure();
	_declspec(dllimport) void testSHA256();
#ifdef __cplusplus
}
#endif