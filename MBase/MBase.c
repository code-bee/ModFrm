// MBase.cpp : 定义 DLL 应用程序的入口点。
//

#include "MBase.h"

//#if (defined _WINDOWS) || (defined WIN32)
#ifdef __M_CFG_OS_WINDOWS

#include <windows.h>

#pragma comment(lib, "../lib/pthread.lib")
#pragma comment(lib, "ws2_32.lib")

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	lpReserved = lpReserved;
	hModule = hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#endif