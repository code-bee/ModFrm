/* 
	M_os_misc.c : 一些os相关的杂项函数

	kevin.zheng@gmail.com
	2012/09/24
*/

#include "../MBase_priv.h"
#include "MBase.h"

#ifdef __M_CFG_OS_LINUX
#include <unistd.h>
#include <sys/syscall.h>
#endif

INLINE	M_tid_t	M_get_tid()
{
#ifdef __M_CFG_OS_WINDOWS
	return (M_tid_t)GetCurrentThreadId();
#endif

#ifdef __M_CFG_OS_LINUX
	return (M_tid_t)syscall(SYS_gettid);
#endif
}
