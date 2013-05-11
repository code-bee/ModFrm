/* 
	M_atomic.h : 实现atomic结构

	kevin.zheng@gmail.com
	2012/09/14
*/

#include <assert.h>
#include "../MBase_priv.h"
#include "MBase.h"

INLINE void 	atomic_init(M_atomic* atom)
{
	atom->atom = 0;
#ifndef __M_CFG_ATOMIC
	pthread_mutex_init(&atom->mutex, NULL);
	pthread_rwlock_init(&atom->rwlock, NULL);
#endif
}

INLINE void 	atomic_destroy(M_atomic* atom)
{
	atom = atom;
#ifndef __M_CFG_ATOMIC
	pthread_mutex_destroy(&atom->mutex);
	pthread_rwlock_destroy(&atom->rwlock);
#endif
}

INLINE void 	atomic_add(M_atomic* atom, M_sintptr v)
{
#ifdef __M_CFG_ATOMIC
	ATOM_ADD_INTPTR(atom, v);
#else
	pthread_mutex_lock(&atom->mutex);
	atom->atom += v;
	pthread_mutex_unlock(&atom->mutex);
#endif
}

INLINE void 	atomic_sub(M_atomic* atom, M_sintptr v)
{
#ifdef __M_CFG_ATOMIC
	ATOM_SUB_INTPTR(atom, v);
#else
	pthread_mutex_lock(&atom->mutex);
	atom->atom -= v;
	pthread_mutex_unlock(&atom->mutex);
#endif
}

INLINE void 	atomic_inc(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	ATOM_INC_INTPTR(atom);
#else
	pthread_mutex_lock(&atom->mutex);
	++atom->atom;
	pthread_mutex_unlock(&atom->mutex);
#endif
}

INLINE void 	atomic_dec(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	ATOM_DEC_INTPTR(atom);
#else
	pthread_mutex_lock(&atom->mutex);
	--atom->atom;
	pthread_mutex_unlock(&atom->mutex);
#endif
}

INLINE M_sint32 atomic_addtest(M_atomic* atom, M_sintptr v)
{
#ifdef __M_CFG_ATOMIC
	return ATOM_ADDTEST_INTPTR(atom, v);
#else
	M_sint32 c;

	pthread_mutex_lock(&atom->mutex);
	atom->atom += v;
	c = atom->atom ? 0 : 1;
	pthread_mutex_unlock(&atom->mutex);
	
	return c;
#endif
}
INLINE M_sint32	atomic_subtest(M_atomic* atom, M_sintptr v)
{
#ifdef __M_CFG_ATOMIC
	return ATOM_SUBTEST_INTPTR(atom, v);
#else
	M_sint32 c;

	pthread_mutex_lock(&atom->mutex);
	atom->atom -= v;
	c = atom->atom ? 0 : 1;
	pthread_mutex_unlock(&atom->mutex);
	
	return c;
#endif
}

INLINE M_sint32	atomic_inctest(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	return ATOM_INCTEST_INTPTR(atom);
#else
	M_sint32 c;

	pthread_mutex_lock(&atom->mutex);
	++atom->atom;
	c = atom->atom ? 0 : 1;
	pthread_mutex_unlock(&atom->mutex);
	
	return c;
#endif
}
INLINE M_sint32	atomic_dectest(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	return ATOM_DECTEST_INTPTR(atom);
#else
	M_sint32 c;

	pthread_mutex_lock(&atom->mutex);
	--atom->atom;
	c = atom->atom ? 0 : 1;
	pthread_mutex_unlock(&atom->mutex);
	
	return c;
#endif
}

INLINE M_sintptr atomic_xchg_return(volatile M_atomic* atom, M_sintptr v)
{
#ifdef __M_CFG_ATOMIC
	ATOM_XCHG_INTPTR(atom, v);
	return v;
#else
	M_sintptr tmp;

	pthread_mutex_lock(&atom->mutex);
	tmp = atom->atom;
	atom->atom = v;
	pthread_mutex_unlock(&atom->mutex);

	return tmp;
#endif
}

INLINE void		atomic_xchg(volatile M_atomic* atom, M_sintptr* v)
{
#ifdef __M_CFG_ATOMIC
	M_sintptr val = *v;
	ATOM_XCHG_INTPTR(atom, val);
	*v = val;
#else
	M_sintptr tmp;

	pthread_mutex_lock(&atom->mutex);
	tmp = atom->atom;
	atom->atom = *v;
	*v = tmp;
	pthread_mutex_unlock(&atom->mutex);
#endif
}

INLINE M_sintptr atomic_cmpxchg(M_atomic* atom, M_sintptr old_val, M_sintptr new_val)
{
#ifdef __M_CFG_ATOMIC
	return ATOM_CMPXCHG_INTPTR((M_sintptr*)atom, old_val, new_val);
#else
	pthread_mutex_lock(&atom->mutex);

	if(atom->atom == old_val)
	{
		old_val = atom->atom;
		atom->atom = new_val;
	}
	else
		old_val = atom->atom;

	pthread_mutex_unlock(&atom->mutex);

	return old_val;
#endif
}


INLINE void		atomic_lock(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	while(atomic_cmpxchg(atom, 0, 1))
		M_sleep(0);
#else
	pthread_mutex_lock(&atom->mutex);
	atom->atom = 1;
#endif
}

INLINE M_sint32	atomic_trylock(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	M_sintptr ret = atomic_cmpxchg(atom, 0, 1);
	return ret ? -1 : 0;
#else
	M_sint32 ret = pthread_mutex_trylock(&atom->mutex);
	if(!ret)
		atom->atom = 1;
	return ret;
#endif
}

INLINE void		atomic_unlock(M_atomic* atom)
{
	atom->atom = 0;
#ifndef __M_CFG_ATOMIC
	pthread_mutex_unlock(&atom->mutex);
#endif
}

INLINE void		atomic_rdlock(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	M_sintptr	i = atom->atom;
	M_sintptr	j = i+1;
	while((i < 0) || atomic_cmpxchg(atom, i, j) != i)
	{
		M_sleep(0);
		i = atom->atom;
		j = i+1;
	}
#else
	pthread_rwlock_rdlock(&atom->rwlock);
	pthread_mutex_lock(&atom->mutex);
	++atom->atom;
	pthread_mutex_unlock(&atom->mutex);
#endif
}

INLINE void		atomic_wrlock(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	M_sintptr	i = atom->atom;
	M_sintptr	j = -1;
	while((i != 0) || atomic_cmpxchg(atom, i, j) != i)
	{
		M_sleep(0);
		i = atom->atom;
	} 
#else
	pthread_rwlock_wrlock(&atom->rwlock);
	--atom->atom;
#endif
}

INLINE M_sint32	atomic_tryrdlock(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	M_sintptr	i = atom->atom, j = i + 1;
	if(i >= 0 && atomic_cmpxchg(atom, i, j) == i)
		return 0;
	
	return -1;
#else
	M_sint32 ret = pthread_rwlock_tryrdlock(&atom->rwlock);
	if(!ret)
	{
		pthread_mutex_lock(&atom->mutex);
		++atom->atom;
		pthread_mutex_unlock(&atom->mutex);
	}

	return ret;
#endif
}

INLINE M_sint32	atomic_trywrlock(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	M_sintptr	i=atom->atom, j=-1;
	if(!i && atomic_cmpxchg(atom, i, j) != i)
		return 0;

	return -1;
#else
	M_sint32 ret = pthread_rwlock_trywrlock(&atom->rwlock);
	if(!ret)
		--atom->atom;

	return ret;
#endif
}

INLINE void		atomic_rwunlock(M_atomic* atom)
{
#ifdef __M_CFG_ATOMIC
	M_sintptr	i = atom->atom;
	M_sintptr	j = i < 0 ? 0 : i - 1;
	assert(i);
	while(atomic_cmpxchg(atom, i, j) != i)
	{
		M_sleep(0);
		i = atom->atom;
		j = i < 0 ? 0 : i - 1;
		assert(i);
	}
#else
	pthread_mutex_lock(&atom->mutex);
	atom->atom = atom->atom < 0 ? 0 : atom->atom - 1;
	pthread_mutex_unlock(&atom->mutex);
	pthread_rwlock_unlock(&atom->rwlock);
#endif
}

INLINE void		atomic_sem_init(M_atomic* atom, M_sintptr count)
{
	atom->atom = count;
}
INLINE void		atomic_sem_wait(M_atomic* atom)
{
	M_sintptr	i = atom->atom;
	M_sintptr	j = i - 1;
	assert(i >= 0);
	while((i == 0) || atomic_cmpxchg(atom, i, j) != i)
	{
		M_sleep(0);
		i = atom->atom;
		j = i - 1;
		assert(i >= 0);
	} 
}
INLINE void		atomic_sem_post(M_atomic* atom)
{
	M_sintptr	i = atom->atom;
	M_sintptr	j = i + 1;
	assert(i >= 0);
	while(atomic_cmpxchg(atom, i, j) != i)
	{
		M_sleep(0);
		i = atom->atom;
		j = i + 1;
		assert(i >= 0);
	} 
}
