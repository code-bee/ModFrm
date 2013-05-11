// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 MBASE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// MBASE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

#ifdef _WINDOWS
#define	__M_CFG_OS_WINDOWS
#endif

#ifdef WIN32
#define	__M_CFG_OS_WINDOWS
#endif

#ifdef __M_CFG_OS_WINDOWS

	#ifdef MBASE_EXPORTS
		#define MBASE_API __declspec(dllexport)
	#else
		#define MBASE_API __declspec(dllimport)
	#endif

	//#define __M_CFG_CMPL_MVC

#else

	#define MBASE_API

#endif

#include <stdio.h>

//#include "pthread.h"

#include "M_types.h"
#include "MBase_header/M_os_misc.h"
#include "MBase_header/M_utility.h"
#include "MBase_header/M_list.h"
#include "MBase_header/M_rbtree.h"
#include "MBase_header/M_atomic.h"
#include "MBase_header/M_data_exchanger.h"
#include "MBase_header/M_lqueue.h"
#include "MBase_header/M_hash.h"
#include "MBase_header/M_tls.h"

#include "MBase_header/M_error.h"
