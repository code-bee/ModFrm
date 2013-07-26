/* 
	M_os_misc.h : 一些os相关的杂项函数

	kevin.zheng@gmail.com
	2012/09/24
*/

#ifndef __M_OS_MISC_H__
#define __M_OS_MISC_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __M_CFG_OS_WINDOWS

#include <winsock2.h>
#include <windows.h>

#define M_msleep(x)	Sleep(x)
#define M_sleep(x)	Sleep((x)*1000)
#endif

#ifdef __M_CFG_OS_LINUX
#include <unistd.h>
#define M_msleep(x)	usleep((x)*1000)
#define M_sleep(x)	sleep(x)
#endif


/*
	type define of thread id
*/
typedef M_uintptr	M_tid_t;
/*
	get thread id, independent with pthread library
*/
MBASE_API	INLINE	M_tid_t	M_get_tid();

#ifdef __cplusplus
}
#endif

#endif	//__M_OS_MISC_H__