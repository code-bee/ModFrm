/* 
	M_log.h : a simple logger, support multi thread logging

	kevin.zheng@gmail.com
	2014/02/20
*/
#ifndef __M_LOG_H__
#define __M_LOG_H__

#include <stdio.h>
#include "M_types.h"
//#include "M_atomic.h"


#ifdef __cplusplus
extern "C" {
#endif

#define	LOG_FATAL	0
#define LOG_ERROR	1
#define LOG_WARNING	2
#define	LOG_INFO	3
#define	LOG_DETAIL	4

typedef struct st_log
{
	M_sint32	(*init)(M_self* self, const M_sint8* log_file, M_sint32 log_level);
	void		(*set_log_level)(M_self* self, M_sint32 log_level);
	M_sint32	(*dump_log)(M_self* self, M_sint32 log_level, const M_sint8* fmt, ...);
	void		(*close_log)(M_self* self);

	//M_sint8		log_file[PATHLEN];
	FILE*		fp;
	M_sint8		log_level;
	//M_atomic	lock;
} M_log;

MBASE_API INLINE void M_log_construct(M_log* obj);


#ifdef __cplusplus
}
#endif

#endif	//__M_LOG_H__