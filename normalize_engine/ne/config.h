#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	֧��set��config�ļ�

	�������֣�һ����ͳ��set������ռ䣬��һ���ֶ�set������
	
	����set����Ŀ��ȷ��������Ҫ��̬����set���ڴ棬set�����ݻ��Ǿ�̬����
*/

#ifndef offset_of
#define offset_of(type,member) (size_t)&(((type*)(0))->member)
#endif
typedef int (*config_reader_t)(char *in_string,void *out_value);
typedef void (*config_setter_t)(int value, void* write_address);

#define offset_of_cfg(cfg_type, set_type) (size_t)&(((cfg_type*)(0))->set_type##_cfgs)

#define is_space(ch)	((ch) == ' ' || (ch) == '\t')


/*
	һ��config set���ڲ���Ϣ
*/
typedef struct st_config_set
{
	char*			item_name;
	config_reader_t config_reader;
	size_t			offset;
} config_set_t;

/*
	һ��config�ļ�������config set��һ����Ϣ
	set_name:		set�����֣���[]�а���������
	offset��		set���������û����õ�cfg�����е�ƫ��������ʵ����user_config_set_t��ƫ����
	set_size:		��ʵ����user_config_set_t->cfgs�Ĵ�С
	config_item_arr:��Ӧ����������
	nr_items:		���set�����������Ŀ
*/
typedef struct st_config
{
	char*			set_name;
	size_t			offset;
	size_t			set_size;
	config_set_t*	config_item_arr;
	int				nr_items;
} config_t;

/*
	���û��ã��û�����config set�����ýṹʱ��̳иýṹ
	cfgs��ָ���û������config set���ýṹ���Ǹ�����
	nr_sets������config set���õ���Ŀ
*/

#define	user_config_set_DECLARE(type)\
	type*	type##_cfgs;\
	int		type##_nr_sets

typedef struct st_user_config_set
{
	void*	cfgs;
	int		nr_sets;
} user_config_set_t;

int		read_config(char* cfg_file, config_t* config_array, int nr_config_array, void* cfg);
void	release_config(config_t* config_array, int nr_config_array, void* cfg);

static __inline void int_acc_setter(int dummy, void* write_address)
{
	(*(int*)write_address)++;
}

static __inline int string_reader(char *in_string,void *out_value) 
{ 
	strcpy((char*)out_value,in_string);
	return 0;
}

static __inline int ip_reader(char *in_string,void *out_value)
{
#ifndef WINDOWS
	unsigned int prefix = inet_addr(in_string);
	*(int *)out_value = prefix;
#endif
	return 0;
}

static __inline int port_reader(char* in_string, void* out_value)
{
#ifndef WINDOWS
	*(short *)out_value = htons(atoi(in_string));
#endif
	return 0;
}

static __inline int int_reader(char *in_string,void *out_value)
{
	*(int *)out_value= atoi(in_string);
	return 0;
}

static __inline int long_reader(char *in_string,void *out_value)
{
	*(long *)out_value= atol(in_string);
	return 0;
}

/*
	��delim���е��ַ��ָ�
*/
static __inline char* my_strtok(char* str, char* delim, char* buf)
{
	char* head = NULL;
	char* tail = NULL;
	char* tmphead;
	char* tmpdelim;

	if(str)
	{
		if(*str)
		{
			strcpy(buf + sizeof(char*), str);
			head = buf + sizeof(char*);
		}
	}
	else
		head = *((char**)buf);

	if(head)
	{
		tmphead = head;
		while(*tmphead)
		{
			tmpdelim = delim;
			while(*tmpdelim)
			{
				if(*tmphead == *tmpdelim)
				{
					*tmphead = 0;
					*(char**)buf = tmphead + 1;
					return head;
				}
				++tmpdelim;
			}
			++tmphead;
		}

		*(char**)buf = NULL;
	}

	return head;
}

/*
	һ�α������ҳ�delim�е�һ�����ֵ��κ��ַ�
*/
static __inline char* my_strchr(char* str, char* delim)
{
	char* tmphead = str;
	char* tmpdelim;
	while(*tmphead)
	{
		tmpdelim = delim;
		while(*tmpdelim)
		{
			if(*tmphead == *tmpdelim)
				return tmphead;
			++tmpdelim;
		}
		++tmphead;
	}
	return NULL;
}

/*
	����delim������delim���ָ�
*/
static __inline char* my_stringtok(char* str, char* delim, char* buf)
{
	char* head = NULL;
	char* tail = NULL;
	if(str)
	{
		if(*str)
		{
			tail = strstr(str, delim);
			if(tail - str > strlen(str))
				return NULL;

			strcpy(buf + sizeof(char*), str);
			head = buf + sizeof(char*);
		}
	}
	else
		head = *((char**)buf);

	if(head)
	{
		if( (tail = strstr(head, delim)) )
		{
			*tail = 0;
			*(char**)buf = tail + strlen(delim);
		}
		else
			*(char**)buf = NULL;
	}

	return head;
}

static __inline char* ltrim(char* str)
{
	while(*str && is_space(*str)) ++str;
	return str;
}

static __inline char* rtrim(char* str)
{
	char* tmp = str;
	char* tail;
	while(*tmp)
	{
		if(!is_space(*tmp))
			tail = tmp;
		++tmp;
	}

	*(++tail)=0;
	return str;
}

static __inline char* trim(char* str)
{
	str = ltrim(str);
	return rtrim(str);
}

#endif	//__CONFIG_H__

