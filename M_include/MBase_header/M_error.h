/* 
	M_error.h : 利用TLS实现线程独立的error num

	kevin.zheng@gmail.com
	2012/12/09
*/

#ifndef __M_ERROR_H__
#define __M_ERROR_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif
//
//
///*
//	ERRNO_XXX macroes are used as err_type in M_err_seterrno/M_err_geterrno/M_err_strerror
//*/
//#define ERRNO_MFRM		1
//#define ERRNO_CLIB		2
//#define ERRNO_SOCKET	3
///**************************************************************************
//	universal error system
//**************************************************************************/
//
//typedef M_sint32 (* geterrno_t)();
//typedef M_sint8* (* strerror_t)(M_sint32 err_num);
//
//MBASE_API	M_sint32	M_err_init();
//MBASE_API	M_sint32	M_err_destroy();
//
//MBASE_API	M_sint32	M_err_register(geterrno_t fp_geterrno, strerror_t fp_strerror);		//return err_type
//MBASE_API	M_sint32	M_err_geterrno(M_sint32	err_type);
//
//// if err_no is 0, error system will call fp_geterrno to get error number
//MBASE_API	M_sint32	M_err_seterrno(M_sint32	err_type, M_sint32 err_no);
//MBASE_API	M_sint8*	M_err_strerror(M_sint32 err_type, M_sint32 err_no);
//
///**************************************************************************
//	error system for module frame
//	
//	in usual cases, these two APIs are enough
//**************************************************************************/
//
//MBASE_API	M_sint32	M_error_init();
//MBASE_API	M_sint32	M_error_destroy();

//typedef struct st_errno
//{
//	M_sint32	err_type;
//	M_sint32	err_num;
//} M_errno;

typedef M_sint32	M_errno;

// M_ET_MFRM should always be last one
// 这有两个目的，其一是确定配置数组（保存strerror和getlasterror函数指针）的大小
// 其二是简化调用，在用户看来M_ET_CLIB和M_ET_SOCKET都是错误码，而不是错误类型
// 错误码大于M_ET_MFRM的就是自定义错误，否则根据各自的getlasterror获取真实的错误码
#define M_ET_SUCCESS	0
#define M_ET_CLIB		1
#define M_ET_SOCKET		2
#define M_ET_MFRM		3

// each lib function of ModFrm needs call M_seterrno at exiting
MBASE_API	M_sint32	M_seterrno(M_sint32	err_num);

// similar with GetLastError of Windows. while in Linux, this function is defined as a macro "errno"
MBASE_API	M_errno		M_geterrno();

// 
MBASE_API	const M_sint8*	M_strerror(M_errno err_no);

MBASE_API	M_sint32		M_error_init();
MBASE_API	M_sint32		M_error_destroy();


#ifdef __cplusplus
}
#endif

#endif	//__M_ERROR_H__