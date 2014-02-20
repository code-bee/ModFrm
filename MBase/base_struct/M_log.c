/* 
	M_log.h : a simple logger

	kevin.zheng@gmail.com
	2014/02/20
*/

#include "../MBase_priv.h"
#include "MBase.h"
#include <assert.h>
#include <stdarg.h>

static INLINE	M_sint32	init_log_f(M_log* self, const M_sint8* log_file, M_sint32 log_level)
{
	//strcpy(self->log_file, log_file);
	if( !(self->fp = fopen(log_file, "a+")) )
		return -1;
	self->log_level = log_level;
	
	return 0;
	
}
static INLINE	void		set_log_level_f(M_log* self, M_sint32 log_level)
{
	self->log_level = log_level;
}

static char* str_log_level[LOG_DETAIL+1] = {"LOG_FATAL", "LOG_ERROR", "LOG_WARNING", "LOG_INFO", "LOG_DETAIL"};

static INLINE	M_sint32	dump_log_f(M_log* self, M_sint32 log_level, const M_sint8* fmt, ...)
{
	M_sint32 written_bytes = 0;
	if(self->log_level <= log_level)
	{
		va_list arg_ptr; 
		va_start(arg_ptr,fmt);  //以固定参数的地址为起点确定变参的内存起始地址。
		written_bytes = vfprintf(self->fp, fmt, arg_ptr);
		va_end(arg_ptr);
	}
	return written_bytes;
}
static INLINE	void	close_log_f(M_log* self)
{
	fclose(self->fp);
}

INLINE void M_log_construct(M_log* obj)
{
	obj->init = (M_sint32(*)(M_self*, const M_sint8*, M_sint32))init_log_f;
	obj->set_log_level = (void(*)(M_self*, M_sint32))set_log_level_f;
	obj->dump_log = (M_sint32(*)(M_self*, M_sint32, const M_sint8*, ...))dump_log_f;
	obj->close_log = (void(*)(M_self*))close_log_f;

	//obj->log_file[0] = 0;
	obj->fp = NULL;
	obj->log_level = LOG_INFO;
}