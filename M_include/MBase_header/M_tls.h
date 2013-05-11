/* 
	M_tls.h : tls定义。TLS：thread local storage

	because of poor functionalities of pthread TLS functions, we had to do something...

	limitations of pthread functions:
	1. can't create/delete keys during threads are started
	2. can't free old memory when pthread_setspecific is called, even destroyer is set

	M_tls can solve these 2 problems, but if somebody wants sematics of POSIX standard, just use 
	pthread_key_create/pthread_key_delete/pthread_get_specific/pthread_set_specific......

	

	kevin.zheng@gmail.com
	2012/12/09
*/

#ifndef __M_TLS_H__
#define __M_TLS_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef M_size_t M_tls_key;

MBASE_API	M_sint32	M_tls_init();
MBASE_API	M_sint32	M_tls_destroy();
MBASE_API	M_sint32	M_tls_isinited();

/*
	memory that tls needs is allocated by malloc, 
	while memory user needs can be allocated anywhere, i.e. by malloc, or from pool
*/
MBASE_API	M_sint32	M_tls_create(M_tls_key* key, free_t destroyer);
MBASE_API	M_sint32	M_tls_delete(M_tls_key key);
MBASE_API	void*		M_tls_get(M_tls_key key);
MBASE_API	M_sint32	M_tls_set(M_tls_key key, void* data);

#ifdef __cplusplus
}
#endif

#endif	//__M_TLS_H__