/* 
	UT_Mbase.h : 测试MBase模块

	kevin.zheng@gmail.com
	2012/09/02
*/
#ifndef __UT_MBASE_H__
#define __UT_MBASE_H__

#include "MBase.h"

#ifdef __cplusplus
extern "C" {
#endif



M_sint32	UT_list();
M_sint32	UT_rbtree();
M_sint32	UT_slisthash();
M_sint32	UT_dlisthash();
M_sint32	UT_bsthash();
M_sint32	UT_atomic();
M_sint32	UT_race();

M_sint32	UT_data_exchanger();
M_sint32	UT_lqueue();

M_sint32	UT_mutex();
M_sint32	UT_rwlock();
M_sint32	UT_sem();
M_sint32	UT_tls();

M_sint32	UT_err();

M_sint32	UT_radix_tree();
M_sint32	UT_radix_tree_pool();
M_sint32	UT_radix_multichar_pool();
M_sint32	UT_radix_mata();

#ifdef __cplusplus
}
#endif

#endif	//__UT_MBASE_H__