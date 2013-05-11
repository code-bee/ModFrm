/* 
	M_error.c : 利用TLS实现线程独立的error num

	kevin.zheng@gmail.com
	2012/12/09
*/

#include "../Mbase_priv.h"
#include "MBase.h"

#include <errno.h>

//// 最大支持8种错误类型
//#define MAX_ERR_TYPE	8
//
//// M_error_node is global
//
//typedef struct st_error_node
//{
//	geterrno_t	fp_geterrno;
//	strerror_t	fp_strerror;
//} M_error_node;
//
//// M_tls_error is thread specific
//typedef struct st_tls_error
//{
//	M_sint32	errs[MAX_ERR_TYPE];
//} M_tls_error;
//
//typedef struct st_error
//{
//	M_tls_key		key;		//tls key for M_tls_error
//	M_sint32		error_num;
//	M_error_node	errors[MAX_ERR_TYPE];
//} M_error;
//
//static M_error s_err;
//
//M_sint32	M_err_init()
//{
//	return M_tls_create(&s_err.key, free);
//}
//M_sint32	M_err_destroy()
//{
//	return M_tls_delete(s_err.key);
//}
//
//M_sint32	M_err_register(geterrno_t fp_geterrno, strerror_t fp_strerror)		//return err_type
//{
//	if(!s_err.key)
//		return -1;
//
//	s_err.errors[s_err.error_num].fp_geterrno = fp_geterrno;
//	s_err.errors[s_err.error_num].fp_strerror = fp_strerror;
//	return s_err.error_num++;
//}
//
//static INLINE M_tls_error*	create_set_tls_data()
//{
//	M_tls_error* err_data = (M_tls_error*)M_tls_get(s_err.key);
//
//	if(!err_data)
//	{
//		if( !(err_data = malloc(sizeof(M_tls_error))) )
//			return NULL;
//
//		memset(err_data, 0, sizeof(M_tls_error));
//		if( !M_tls_set(s_err.key, err_data) )
//		{
//			free(err_data);
//			return NULL;
//		}
//	}
//
//	return err_data;
//}
//
//M_sint32	M_err_geterrno(M_sint32	err_type)
//{
//	M_tls_error* err_data = create_set_tls_data();
//
//	if(s_err.errors[err_type].fp_geterrno)
//		err_data->errs[err_type] = s_err.errors[err_type].fp_geterrno();
//	return err_data->errs[err_type];
//}
//
//MBASE_API	M_sint32	M_err_seterrno(M_sint32	err_type, M_sint32 err_no)
//{
//	M_tls_error* err_data = create_set_tls_data();
//
//	if(!err_no)
//		err_data->errs[err_type] = s_err.errors[err_type].fp_geterrno();
//	else
//		err_data->errs[err_type] = err_no;
//	return 0;
//}
//
//M_sint8*	M_err_strerror(M_sint32 err_type, M_sint32 err_no)	//may return NULL string
//{
//	if(s_err.errors[err_type].fp_strerror)
//		return s_err.errors[err_type].fp_strerror(err_no);
//	else
//		return NULL;
//}
//
///**************************************************************************
//	error system for module frame
//**************************************************************************/
//
//M_sint32 et_clib;
//M_sint32 et_mfrm;
//
//static INLINE M_sint32 clib_geterrno()
//{
//	return errno;
//}
//
//static INLINE M_sint8* clib_strerror(M_sint32 err_no)
//{
//	return strerror(err_no);
//}
//
//MBASE_API	M_sint32	M_error_init()
//{
//	M_sint32 ret = M_err_init();
//
//	if(ret)
//		return ret;
//
//	et_clib = M_err_register(clib_geterrno, clib_strerror);
//	et_mfrm = M_err_register(NULL, M_strerror);
//
//
//}
//
//MBASE_API	M_sint32	M_error_destroy()
//{}


#define M_ET_NUM	(M_ET_MFRM+1)

typedef M_sint32 (* geterrno_t)();
typedef M_sint8* (* strerror_t)(M_sint32 err_num);

typedef struct st_error_node
{
	geterrno_t	fp_geterrno;
	strerror_t	fp_strerror;
} M_error_node;

static M_tls_key	M_tls_err_key;

static INLINE M_sint32	clib_geterrno()
{
	return errno;
}
static INLINE M_sint8*	clib_strerror(M_sint32 err_num)
{
	return strerror(err_num);
}

static INLINE M_sint32	socket_geterrno()
{
#ifdef __M_CFG_OS_LINUX
	return errno;
#elif (defined __M_CFG_OS_WINDOWS)
	return WSAGetLastError();
#endif
}

static INLINE M_sint8*	socket_strerror(M_sint32 err_num)
{
#ifdef __M_CFG_OS_LINUX
	return strerror(err_num);
#elif (defined __M_CFG_OS_WINDOWS)
	switch(err_num)
	{
	case WSAEINTR:
		break;

	case WSAEACCES:
		break;

		//TODO...
	}

	return NULL;
#endif
}

static INLINE M_sint32	frm_geterrno()
{
	//TODO...  just return tls err_num
	return 0;
}
static INLINE M_sint8*	frm_strerror(M_sint32 err_num)
{
	//TODO...  write a big switch/case for error description
	return NULL;
}

static M_error_node	s_errs[M_ET_NUM] =
{
	{NULL, NULL},							// 占个位子...
	{clib_geterrno,		clib_strerror},		//general clib
	{socket_geterrno,	socket_strerror},	//socket
	{frm_geterrno,		frm_strerror}		//module frame
};

static INLINE M_errno	get_tls_errno()
{
	M_errno* err_data = (M_errno*)M_tls_get(M_tls_err_key);

	if(!err_data)
	{
		if( !(err_data = malloc(sizeof(M_errno))) )
			return NULL;

		memset(err_data, 0, sizeof(M_errno));
		if( !M_tls_set(M_tls_err_key, err_data) )
		{
			free(err_data);
			return NULL;
		}
	}

	return *err_data;
}

M_sint32	M_seterrno(M_sint32	err_num)
{
	M_errno* err_data = get_tls_errno();

	if(err_data)
	{
		*err_data = err_num;
		return 0;
	}

	return -1;
}

M_errno	M_geterrno()
{
	M_errno* err_data = get_tls_errno();

	if(err_data)
	{
		switch(*err_data)
		{
		case M_ET_CLIB:
		case M_ET_SOCKET:
			return s_errs[*err_data].fp_geterrno();
		default:
			return *err_data;
		}
	}
	else
		return -1;
}
 
const M_sint8*	M_strerror(M_errno err_num)
{
	switch(err_num)
	{
	case M_ET_SUCCESS:
		return NULL;
	case M_ET_CLIB:
	case M_ET_SOCKET:
		return s_errs[err_num].fp_strerror(s_errs[err_num].fp_geterrno());
	default:
		return s_errs[err_num].fp_strerror(err_num);
	}
}

M_sint32		M_error_init()
{
	return M_tls_create(&M_tls_err_key, free);
}

M_sint32		M_error_destroy()
{
	return M_tls_delete(M_tls_err_key);
}