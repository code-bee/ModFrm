/* 
	M_asm_x86.h : 定义/实现x86 CPU的汇编

	kevin.zheng@gmail.com
	2012/09/14
*/

#ifndef __M_ASM_X86_H__
#define __M_ASM_X86_H__

#include "M_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __M_CFG_CPU_X86

#if	(defined __M_CFG_CMPL_MVC)

#ifdef __M_CFG_SMP
	#define LOCK_PREFIX lock
#else
	#define LOCK_PREFIX 
#endif

#ifndef __M_CFG_OS64

#define ATOM_ADD32_X86_MVC(x, v)\
	__asm {\
		__asm mov ecx, x \
		__asm mov edx, v \
		__asm LOCK_PREFIX add dword ptr [ecx], edx \
	}

#define ATOM_SUB32_X86_MVC(x, v)\
	__asm {\
		__asm mov ecx, x \
		__asm mov edx, v \
		__asm LOCK_PREFIX sub dword ptr [ecx], edx \
	}

#define ATOM_INC32_X86_MVC(x)\
	__asm {\
		__asm mov ecx, x \
		__asm LOCK_PREFIX inc dword ptr [ecx] \
	}

#define ATOM_DEC32_X86_MVC(x)\
	__asm {\
		__asm mov ecx, x \
		__asm LOCK_PREFIX dec dword ptr [ecx] \
	}

/*
	x is a pointer to integer or other pointer
	v is an integer or pointer
	xchg result: *x --> v, v --> *x
	memory where x points is locked
	v is a local variable
*/
#define ATOM_XCHG32_X86_MVC(x, v)\
	__asm {\
		__asm mov ecx, x \
		__asm mov edx, v \
		__asm LOCK_PREFIX xchg dword ptr [ecx], edx \
		__asm mov v, edx \
	}

static INLINE M_sint32 ATOM_ADDTEST32_X86_MVC(M_sint32* x, M_sint32 v)
{
	M_uint8 c;
	
	__asm {
		__asm mov ecx, x 
		__asm mov edx, v 
		__asm LOCK_PREFIX add dword ptr [ecx], edx 
		__asm sete c
	}

	return c != 0;
}

static INLINE M_sint32 ATOM_SUBTEST32_X86_MVC(M_sint32* x, M_sint32 v)
{
	M_uint8 c;
	
	__asm {
		__asm mov ecx, x 
		__asm mov edx, v 
		__asm LOCK_PREFIX sub dword ptr [ecx], edx 
		__asm sete c
	}

	return c != 0;
}

static INLINE M_sint32 ATOM_INCTEST32_X86_MVC(M_sint32* x)
{
	M_uint8 c;
	
	__asm {
		__asm mov ecx, x 
		__asm LOCK_PREFIX inc dword ptr [ecx]
		__asm sete c
	}

	return c != 0;
}

static INLINE M_sint32 ATOM_DECTEST32_X86_MVC(M_sint32* x)
{
	M_uint8 c;
	
	__asm {
		__asm mov ecx, x 
		__asm LOCK_PREFIX dec dword ptr [ecx] 
		__asm sete c
	}

	return c != 0;
}

/*
	compare *x with oldv, 
	if equal, then *x = newv, return old *x(i.e. oldv)
	else, do nothing, return *x

	cmpxchg: compare ecx with first operand, i.e. *x
	if equal, load second operand to first operand, i.e. set *x = newv (ecx is not changed, still be oldv, and oldv is just *x)
	else, load first operand to ecx, i.e., set ecx = *x (ecx is *x now)

	so in all cases, when cmpxchg execute over, ecx has the return value of our wrapper function

	according to Intel convention, ecx is retrun value of function, 
	so last instruction is not necessary, same with variable prev -- it is not necessasry too

*/
static INLINE M_sint32 ATOM_CMPXCHG32_X86_MVC(M_sint32* x, M_sint32 oldv, M_sint32 newv)
{
	//M_sint32 prev = *x;

	__asm {
		__asm mov eax, oldv
		__asm mov ebx, x
		__asm mov ecx, newv
		__asm LOCK_PREFIX cmpxchg dword ptr [ebx], ecx
		//__asm mov prev, eax
	}

	//return prev;
}

#else //OS_64

#define ATOM_ADD64_X86_MVC(x, v)\
	__asm {\
		__asm mov rcx, x \
		__asm mov rdx, v \
		__asm LOCK_PREFIX add qword ptr [rcx], rdx \
	}

#define ATOM_SUB64_X86_MVC(x, v)\
	__asm {\
		__asm mov rcx, x \
		__asm mov rdx, v \
		__asm LOCK_PREFIX sub qword ptr [rcx], rdx \
	}

#define ATOM_INC64_X86_MVC(x)\
	__asm {\
		__asm mov rcx, x \
		__asm LOCK_PREFIX inc qword ptr [rcx] \
	}

#define ATOM_DEC64_X86_MVC(x)\
	__asm {\
		__asm mov rcx, x \
		__asm LOCK_PREFIX dec qword ptr [rcx] \
	}

/*
	x is a pointer to integer or other pointer
	v is an integer or pointer
	xchg result: *x --> v, v --> *x
	memory where x points is locked
	v is a local variable
*/
#define ATOM_XCHG64_X86_MVC(x, v)\
	__asm {\
		__asm mov rcx, x \
		__asm mov rdx, v \
		__asm LOCK_PREFIX xchg qword ptr [rcx], rdx \
		__asm mov v, rdx \
	}

static INLINE M_sint64 ATOM_ADDTEST64_X86_MVC(M_sint64* x, M_sint64 v)
{
	M_uint8 c;
	
	__asm {
		__asm mov rcx, x 
		__asm mov rdx, v 
		__asm LOCK_PREFIX add qword ptr [rcx], rdx 
		__asm sete c
	}

	return c != 0;
}

static INLINE M_sint64 ATOM_SUBTEST64_X86_MVC(M_sint64* x, M_sint64 v)
{
	M_uint8 c;
	
	__asm {
		__asm mov rcx, x 
		__asm mov rdx, v 
		__asm LOCK_PREFIX sub qword ptr [rcx], rdx 
		__asm sete c
	}

	return c != 0;
}

static INLINE M_sint64 ATOM_INCTEST64_X86_MVC(M_sint64* x)
{
	M_uint8 c;
	
	__asm {
		__asm mov rcx, x 
		__asm LOCK_PREFIX inc qword ptr [rcx]
		__asm sete c
	}

	return c != 0;
}

static INLINE M_sint64 ATOM_DECTEST64_X86_MVC(M_sint64* x)
{
	M_uint8 c;
	
	__asm {
		__asm mov rcx, x 
		__asm LOCK_PREFIX dec qword ptr [rcx] 
		__asm sete c
	}

	return c != 0;
}

/*
	compare *x with oldv, 
	if equal, then *x = newv, return old *x(i.e. oldv)
	else, do nothing, return *x

	cmpxchg: compare ecx with first operand, i.e. *x
	if equal, load second operand to first operand, i.e. set *x = newv (ecx is not changed, still be oldv, and oldv is just *x)
	else, load first operand to ecx, i.e., set ecx = *x (ecx is *x now)

	so in all cases, when cmpxchg execute over, ecx has the return value of our wrapper function

	according to Intel convention, ecx is retrun value of function, 
	so last instruction is not necessary, same with variable prev -- it is not necessasry too

*/
static INLINE M_sint64 ATOM_CMPXCHG64_X86_MVC(M_sint64* x, M_sint64 oldv, M_sint64 newv)
{
	//M_sint32 prev = *x;

	__asm {
		__asm mov rax, oldv
		__asm mov rbx, x
		__asm mov rcx, newv
		__asm LOCK_PREFIX cmpxchg qword ptr [rbx], rcx
		//__asm mov prev, eax
	}

	//return prev;
}
#endif // OS_64

#elif (defined __M_CFG_CMPL_GCC)
#ifdef __M_CFG_SMP
	#define LOCK "lock ; "
#else
	#define LOCK ""
#endif



#ifndef __M_CFG_OS64

// add v to *x
#define ATOM_ADD32_X86_GCC(x, v)\
	asm volatile( LOCK "addl %1,%0" \
	: "+m" (*(M_sint32*)x) \
	: "ir" (v) )

// sub v from *x
#define ATOM_SUB32_X86_GCC(x, v)\
	asm volatile( LOCK "subl %1,%0" \
	: "+m" (*(M_sint32*)x) \
	: "ir" (v) )

// inc *x
#define ATOM_INC32_X86_GCC(x)\
	asm volatile( LOCK "incl %0" \
	: "+m" (*(M_sint32*)x) )

// dec *x
#define ATOM_DEC32_X86_GCC(x)\
	asm volatile( LOCK "decl %0" \
	: "+m" (*(M_sint32*)x) )

//static inline int xchg(volatile void *x, unsigned long v, int size)
//{
//	switch (size) {
//	case 1:
//		{
//			asm volatile("xchgb %0,%1"
//				: "=q" (v), "+m" (*(M_sint8*)x)
//				: "0" (v)
//				: "memory");
//			break;
//		}
//	case 2:
//		{
//			asm volatile("xchgw %0,%1"
//				: "=q" (v), "+m" (*(M_sint16*)x)
//				: "0" (v)
//				: "memory");
//			break;
//		}
//	case 4:
//		{
//			asm volatile("xchgl %0,%1"
//				: "=q" (v), "+m" (*(M_sint32*)x)
//				: "0" (v)
//				: "memory");
//			break;
//		}
//	default:
//		return -1;
//	}   
//
//	return 0;
//}

/*
	x is a pointer to integer or other pointer
	v is an integer or pointer
	xchg result: *x --> v, v --> *x
	memory where x points is locked
	v is a local variable
*/
#define ATOM_XCHG32_X86_GCC(x, v) \
			asm volatile("xchgl %0,%1" \
				: "=q" (v), "+m" (*(M_sint32*)x) \
				: "0" (v) \
				: "memory")


static INLINE M_sint32 ATOM_ADDTEST32_X86_GCC(M_sint32* x, M_sint32 v)
{
	M_uint8 c;

	asm volatile(
		LOCK "addl %2,%0; sete %1"
		:"+m" (*(M_sint32*)x), "=qm" (c)
		:"ir" (v) : "memory");
	return c; //!= 0
}

static INLINE M_sint32 ATOM_SUBTEST32_X86_GCC(M_sint32* x, M_sint32 v)
{
	M_uint8 c;

	asm volatile(
		LOCK "subl %2,%0; sete %1"
		:"+m" (*(M_sint32*)x), "=qm" (c)
		:"ir" (v) : "memory");
	return c; //!= 0
}

static INLINE M_sint32 ATOM_INCTEST32_X86_GCC(M_sint32* x)
{
	M_uint8 c;

	asm volatile(
		LOCK "incl %0; sete %1"
		:"+m" (*(M_sint32*)x), "=qm" (c)
		: : "memory");
	return c;// != 0;
}

static INLINE M_sint32 ATOM_DECTEST32_X86_GCC(M_sint32* x)
{
	M_uint8 c;

	asm volatile(
		LOCK "decl %0; sete %1"
		:"+m" (*(M_sint32*)x), "=qm" (c)
		: : "memory");
	return c;// != 0;
}


//static inline unsigned long cmpxchg(volatile void *x, unsigned long old, unsigned long new, int size)
//{
//    unsigned long prev;
//    switch (size) {
//    case 1:
//        __asm__ __volatile__(LOCK_PREFIX "cmpxchgb %1,%2"
//                     : "=a"(prev)
//                     : "q"(new), "m"(*(M_sint8*)x), "0"(old)
//                     : "memory");
//        return prev;
//    case 2:
//        __asm__ __volatile__(LOCK_PREFIX "cmpxchgw %1,%2"
//                     : "=a"(prev)
//                     : "r"(new), "m"(*(M_sint16*)x), "0"(old)
//                     : "memory");
//        return prev;
//    case 4:
//        __asm__ __volatile__(LOCK_PREFIX "cmpxchgl %1,%2"
//                     : "=a"(prev)
//                     : "r"(new), "m"(*(M_sint32*)x), "0"(old)
//                     : "memory");
//        return prev;
//    }
//    return old;
//}

/*
	compare *x with oldv, 
	if equal, then *x = newv, return old *x(i.e. oldv)
	else, do nothing, return *x

	cmpxchg: compare ecx with first operand, i.e. *x
	if equal, load second operand to first operand, i.e. set *x = newv (ecx is not changed, still be oldv, and oldv is just *x)
	else, load first operand to ecx, i.e., set ecx = *x (ecx is *x now)

	so in all cases, when cmpxchg execute over, ecx has the return value of our wrapper function

	according to Intel convention, ecx is retrun value of function, 
	so last instruction is not necessary, same with variable prev -- it is not necessasry too

*/
static INLINE M_sint32 ATOM_CMPXCHG32_X86_GCC(M_sint32* x, M_sint32 oldv, M_sint32 newv)
{
	M_sint32 prev;
	__asm__ __volatile__(LOCK "cmpxchgl %1,%2"
		: "=a"(prev)
		: "r"(newv), "m"(*(M_sint32*)x), "0"(oldv)
		: "memory");
	return prev;
}


#else //OS_64

// add v to *x
#define ATOM_ADD64_X86_GCC(x, v)\
	asm volatile( LOCK "addq %1,%0" \
	: "+m" (*(M_sint64*)x) \
	: "ir" (v) )

// sub v from *x
#define ATOM_SUB64_X86_GCC(x, v)\
	asm volatile( LOCK "subq %1,%0" \
	: "+m" (*(M_sint64*)x) \
	: "ir" (v) )

// inc *x
#define ATOM_INC64_X86_GCC(x)\
	asm volatile( LOCK "incq %0" \
	: "+m" (*(M_sint64*)x) )

// dec *x
#define ATOM_DEC64_X86_GCC(x)\
	asm volatile( LOCK "decq %0" \
	: "+m" (*(M_sint64*)x) )
/*
	x is a pointer to integer or other pointer
	v is an integer or pointer
	xchg result: *x --> v, v --> *x
	memory where x points is locked
	v is a local variable
*/
#define ATOM_XCHG64_X86_GCC(x, v) \
			asm volatile("xchgq %0,%1" \
				: "=q" (v), "+m" (*(M_sint64*)x) \
				: "0" (v) \
				: "memory")


static INLINE M_sint64 ATOM_ADDTEST64_X86_GCC(M_sint64* x, M_sint64 v)
{
	M_uint8 c;

	asm volatile(
		LOCK "addq %2,%0; sete %1"
		:"+m" (*(M_sint64*)x), "=qm" (c)
		:"ir" (v) : "memory");
	return c; //!= 0
}

static INLINE M_sint64 ATOM_SUBTEST64_X86_GCC(M_sint64* x, M_sint64 v)
{
	M_uint8 c;

	asm volatile(
		LOCK "subq %2,%0; sete %1"
		:"+m" (*(M_sint64*)x), "=qm" (c)
		:"ir" (v) : "memory");
	return c; //!= 0
}

static INLINE M_sint64 ATOM_INCTEST64_X86_GCC(M_sint64* x)
{
	M_uint8 c;

	asm volatile(
		LOCK "incq %0; sete %1"
		:"+m" (*(M_sint64*)x), "=qm" (c)
		: : "memory");
	return c;// != 0;
}

static INLINE M_sint64 ATOM_DECTEST64_X86_GCC(M_sint64* x)
{
	M_uint8 c;

	asm volatile(
		LOCK "decq %0; sete %1"
		:"+m" (*(M_sint64*)x), "=qm" (c)
		: : "memory");
	return c;// != 0;
}
/*
	compare *x with oldv, 
	if equal, then *x = newv, return old *x(i.e. oldv)
	else, do nothing, return *x

	cmpxchg: compare ecx with first operand, i.e. *x
	if equal, load second operand to first operand, i.e. set *x = newv (ecx is not changed, still be oldv, and oldv is just *x)
	else, load first operand to ecx, i.e., set ecx = *x (ecx is *x now)

	so in all cases, when cmpxchg execute over, ecx has the return value of our wrapper function

	according to Intel convention, ecx is retrun value of function, 
	so last instruction is not necessary, same with variable prev -- it is not necessasry too

*/
static INLINE M_sint64 ATOM_CMPXCHG64_X86_GCC(M_sint64* x, M_sint64 oldv, M_sint64 newv)
{
	M_sint64 prev;
	__asm__ __volatile__(LOCK "cmpxchgq %1,%2"
		: "=a"(prev)
		: "r"(newv), "m"(*(M_sint64*)x), "0"(oldv)
		: "memory");
	return prev;
}
#endif // OS_64

#endif	//end compiler

#endif	//__M_CFG_CPU_X86

#ifdef __cplusplus
}
#endif

#endif	//__M_ASM_X86_H__
