/* 
	M_types.h : 定义基本平台类型

	kevin.zheng@gmail.com
	2012/09/01
*/
#ifndef __M_TYPES_H__
#define __M_TYPES_H__

//#include <stdint.h>

/*
	Preprocessers:
		__M_CFG_OS64:				defined if OS is 64 bit
		__M_CFG_OS_WINDOWS:			defined if OS is windows series
		__M_CFG_OS_LINUX:			defined if OS is linux series

		__M_CFG_CMPL_MVC:			defined if compiler is Microsoft Visual C++ compiler
		__M_CFG_CMPL_GCC:			defined if compiler is gcc

		__M_CFG_CPU_X86:			defined if cpu is x86 architecture
		__M_CFG_SMP:				defined if multicore is presented

		__M_CFG_ATOMIC:				defined if cpu supports atomic operations, including xchg, cmpxchg

		__M_CFG_COUNT_LQUEUE:		count series macro, if defined, provides node counts in M_lqueue

		__M_CFG_POOL_LEAK:			activate a simple memory leak detector by M_poolinf

	inherited from MS Windows:
		_DEBUG
		WIN32
		_WINDOWS
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#define	INLINE		__inline	

#define __M_CFG_CPU_X86
#ifdef __M_CFG_CPU_X86
	#define __M_CFG_ATOMIC
#endif

#ifdef __M_CFG_OS_WINDOWS
	#define __M_CFG_CMPL_MVC
#endif

#ifdef __M_CFG_OS_LINUX
	#define __M_CFG_CMPL_GCC
#endif

#define	__M_CFG_SMP
#define __M_CFG_POOL_LEAK

typedef unsigned char	M_uint8;
typedef signed char		M_sint8;
typedef unsigned short	M_uint16;
typedef signed short	M_sint16;
typedef unsigned int	M_uint32;
typedef signed int		M_sint32;
typedef void			M_self;

#if (defined _WINDOWS) || (defined WIN32)
	typedef __int64				M_sint64;
	typedef unsigned __int64	M_uint64;
#else
	typedef long long			M_sint64;
	typedef unsigned long long	M_uint64;
#endif

#ifdef __M_CFG_OS64
	typedef M_sint64		M_sintptr;
	typedef M_uint64		M_uintptr;
#else
	typedef M_sint32		M_sintptr;
	typedef M_uint32		M_uintptr;
#endif

typedef M_sint32		M_size;
typedef M_uintptr		M_size_t;

#define offset_of(type, member)			( (M_sint8*)(&((type*)0)->member) )
#define container_of(ptr, type, member)	( (type*)((M_sint8*)ptr - offset_of(type, member)) )

typedef struct st_string
{
	M_sint8*	str;
	M_sint32	str_len;
} M_string;


/*
	general function types

	cmp_key_t:	return value:	0	-- key1 == key2
								1	-- key1 >  key2
								-1	-- key2 <  key2
	
	get_key_t:	get key via structure stub

	get_rbcolor_t:	get color via red black tree stub
	set_rbcolor_t:	set color via red black tree stub. color is either RB_RED or RB_BLACK
*/

#define RB_RED		0
#define	RB_BLACK	1
typedef M_sintptr	(* cmp_key_t)(void* key1, void* key2);
typedef void*		(* get_key_t)(void* stub);
typedef	M_sint32	(* get_rbcolor_t)(void* stub);
typedef	void		(* set_rbcolor_t)(void* stub, M_sint32 color);

static INLINE M_sintptr	cmp_key_M_sint8(void* key1, void* key2)
{
	return (M_sintptr)(*(M_sint8*)key1 - *(M_sint8*)key2);
}

static INLINE M_sintptr	cmp_key_M_sint32(void* key1, void* key2)
{
	return (M_sintptr)(*(M_sint32*)key1 - *(M_sint32*)key2);
}

static INLINE M_sintptr	cmp_key_M_uint32(void* key1, void* key2)
{
	return (*(M_uint32*)key1 > *(M_uint32*)key2) ? 1 : ((*(M_uint32*)key1 < *(M_uint32*)key2) ? -1 : 0);
}

static INLINE M_sintptr	cmp_key_M_sintptr(void* key1, void* key2)
{
	return *(M_sintptr*)key1 - *(M_sintptr*)key2;
}

static INLINE M_sintptr	cmp_key_M_uintptr(void* key1, void* key2)
{
	return (*(M_uintptr*)key1 > *(M_uintptr*)key2) ? 1 : ((*(M_uintptr*)key1 < *(M_uintptr*)key2) ? -1 : 0);
}

static INLINE M_sintptr	cmp_key_void_ptr(void* key1, void* key2)
{
	return (key1 == key2) ? 0 : (key1 > key2) ? 1 : -1;
	//if((M_sint8*)key1 == (M_sint8*)key2)
	//	return 0;
	//else
	//	return ((M_sint8*)key1 > (M_sint8*)key2) ? 1 : -1;
}

typedef void*	(* M_malloc_t)(M_size_t size, void* pool);
typedef void	(* M_free_t)(void* memblock, void* pool);

typedef void*	(* malloc_t)(M_size_t size);
typedef void	(* free_t)(void* memblock);

typedef void	(* traveller_t)(void* node, void* param);

static INLINE	void*	M_malloc(M_size_t size, void* pool)
{
	pool = pool;
	return malloc(size);
}

static INLINE	void	M_free(void* mem, void* pool)
{
	pool = pool;
	free(mem);
}

#ifdef __cplusplus
}
#endif

#endif	//__M_TYPES_H__
