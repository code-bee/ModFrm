/* 
	ut_types.c : 测试宏

	kevin.zheng@gmail.com
	2012/09/02
*/

#include "ut_types.h"

/*
	cond: expression to be asserted, 0 -- fail, 1 -- OK
	print flag: 0 -- does not print successful tests
				1 -- print successful tests
*/
M_sint32	ut_assert(M_sint32 cond, M_sint32 print_OK, FILE* fp, const M_sint8* file_name, const M_sint8* func_name, M_sint32 line)
{
	if(cond)
	{
		if(print_OK)
			fprintf(fp, "Test OK at line %d of %s(), in file %s\n", line, func_name, file_name); 
		return 1;
	}
	else
	{
		fprintf(fp, "Test FAIL at line %d of %s(), in file %s\n", line, func_name, file_name); 
		return 0;
	}

}
/*
	similar to ut_assert, but never print successful tests
	if test fails, and print_fail is true, then print fail message;
	otherwise print nothing
*/
M_sint32	ut_check(M_sint32 cond, M_sint32 print_fail, FILE* fp, const M_sint8* file_name, const M_sint8* func_name, M_sint32 line)
{
	if(!cond)
	{
		if(print_fail)
			fprintf(fp, "Test FAIL at line %d of %s(), in file %s\n", line, func_name, file_name); 
		return 1;
	}
	else
		return 0;
}

void	ut_conclude(M_sint32 cond, FILE* fp, const M_sint8* func_name)
{
	if(cond)
		fprintf(fp, "OK! %s\n\n", func_name);
	else
		fprintf(fp, "Fail! %s\n\n", func_name);
}