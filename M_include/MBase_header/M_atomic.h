/* 
	M_atomic.h : 定义atomic结构

	kevin.zheng@gmail.com
	2012/09/14
*/

#ifndef __M_ATOMIC_H__
#define __M_ATOMIC_H__

#ifndef __M_CFG_ATOMIC
#include "pthread.h"
#endif

#include "M_types.h"
#include "M_asm.h"

#ifdef __cplusplus
extern "C" {
#endif

//typedef struct st_atomic_32
//{
//	M_sint32	atom;
//#ifndef __M_CFG_ATOMIC
//	pthread_mutex_t	mutex;
//#endif
//} M_atomic_32;

typedef struct st_atomic_ptr
{
	volatile M_sintptr	atom;
#ifndef __M_CFG_ATOMIC
	pthread_mutex_t		mutex;
	pthread_rwlock_t	rwlock;
#endif
} M_atomic_ptr;

typedef M_atomic_ptr	M_atomic;

/*
	init: set atom 0, and init mutex if necessary
	destory: destory mutex if necessary
*/
MBASE_API INLINE void		atomic_init(M_atomic* atom);
MBASE_API INLINE void		atomic_destroy(M_atomic* atom);

/*
	basic operations
	addtest, subtest: add/sub, then test result if zero(true) or not(false)
	inctest, dectest: inc/dec, then test result if zero(true) or not(false)
*/
MBASE_API INLINE void		atomic_add(M_atomic* atom, M_sintptr v);
MBASE_API INLINE void		atomic_sub(M_atomic* atom, M_sintptr v);
MBASE_API INLINE M_sint32	atomic_addtest(M_atomic* atom, M_sintptr v);
MBASE_API INLINE M_sint32	atomic_subtest(M_atomic* atom, M_sintptr v);

MBASE_API INLINE void		atomic_inc(M_atomic* atom);
MBASE_API INLINE void		atomic_dec(M_atomic* atom);
MBASE_API INLINE M_sint32	atomic_inctest(M_atomic* atom);
MBASE_API INLINE M_sint32	atomic_dectest(M_atomic* atom);

/*
	result: v --> atom->atom
	return old atom->atom
	
	usually it is used in following way to avoid transferring address of v:
	v = atomic_xchg_return(atom, v);
*/
MBASE_API INLINE M_sintptr	atomic_xchg_return(volatile M_atomic* atom, M_sintptr v);

/*
	this version uses address of v, so it is used in following way:
	atomic_xchg(atom, v), where v is a pointer
*/
MBASE_API INLINE void		atomic_xchg(volatile M_atomic* atom, M_sintptr* v);

MBASE_API INLINE M_sintptr	atomic_cmpxchg(M_atomic* atom, M_sintptr old_val, M_sintptr new_val);

/*
	atmoic locks, be aware of starvation

	trylock version: return 0 if successful get the lock, -1 otherwise
*/
MBASE_API INLINE void		atomic_lock(M_atomic* atom);
MBASE_API INLINE M_sint32	atomic_trylock(M_atomic* atom);
MBASE_API INLINE void		atomic_unlock(M_atomic* atom);

MBASE_API INLINE void		atomic_rdlock(M_atomic* atom);
MBASE_API INLINE void		atomic_wrlock(M_atomic* atom);
MBASE_API INLINE M_sint32	atomic_tryrdlock(M_atomic* atom);
MBASE_API INLINE M_sint32	atomic_trywrlock(M_atomic* atom);
MBASE_API INLINE void		atomic_rwunlock(M_atomic* atom);

MBASE_API INLINE void		atomic_sem_init(M_atomic* atom, M_sintptr count);
MBASE_API INLINE void		atomic_sem_wait(M_atomic* atom);
MBASE_API INLINE void		atomic_sem_post(M_atomic* atom);


#ifdef __cplusplus
}
#endif

#endif	//__M_ATOMIC_H__