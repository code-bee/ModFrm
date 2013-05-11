/* 
	ut_types.h : 测试宏

	kevin.zheng@gmail.com
	2012/09/02
*/
#ifndef __UT_TYPES_H__
#define __UT_TYPES_H__

#include "MBase.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UT_ASSERT(a) ut_assert(a, 0, stdout, __FILE__, __FUNCTION__, __LINE__)
#define UT_CHECK(a) ut_check(a, 0, stdout, __FILE__, __FUNCTION__, __LINE__)
#define UT_CONCLUDE(a) ut_conclude(a, stdout, __FUNCTION__)

/*
	cond: expression to be asserted, 0 -- fail, 1 -- OK
	print flag: 0 -- does not print successful tests
				1 -- print successful tests
*/
M_sint32	ut_assert(M_sint32 cond, M_sint32 print_OK, FILE* fp, const M_sint8* file_name, const M_sint8* func_name, M_sint32 line);
/*
	similar to ut_assert, but never print successful tests
	if test fails, and print_fail is true, then print fail message;
	otherwise print nothing
*/
M_sint32	ut_check(M_sint32 cond, M_sint32 print_fail, FILE* fp, const M_sint8* file_name, const M_sint8* func_name, M_sint32 line);

void		ut_conclude(M_sint32 cond, FILE* fp, const M_sint8* func_name);

#ifdef __cplusplus
}
#endif

#endif	//__UT_TYPES_H__