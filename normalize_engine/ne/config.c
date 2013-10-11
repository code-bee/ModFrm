#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#ifndef WIN32
#include <arpa/inet.h>
#endif
#include "config.h"
#include "acsmx2.h"

#define LINE_SIZE 1024

//char* my_strtok(char* str, char* delim, char* buf);
//char* ltrim(char* str);
//char* rtrim(char* str);
//char* trim(char* str);

//强制转换第i个全局配置成员为user_config_set_t类型
#define get_set_cfg(cfg, i)						((user_config_set_t*)((char*)(cfg) + config_array[i].offset))

/*
	两次遍历，第一次获得各个set的数目，分配空间
	第二次才是真正的读入配置

	由于需要分配空间，所以必须有release_config方法
*/
int read_config(char *cfg_file, config_t* config_array, int nr_config_array, void* cfg, void (*default_cfg)(void* cfgs, M_stackpool* pool), M_stackpool* pool)
{
	FILE *fp = NULL;
	char buf[LINE_SIZE]; //缓冲区数组
	//char buf_[LINE_SIZE];
	char *pitem = NULL;
	char *pvalue = NULL;
	char *tmp;
	config_t*			current_set = NULL;
	user_config_set_t*	current_set_cfg = NULL;
	void*				current_item_cfg = NULL;
	
	int i = 0;
	int line_nr = 0;

	if( !(fp = fopen(cfg_file,"r")) )
	{
		PRINTF("open file %s fail, %s\n", cfg_file, strerror(errno));
		return -1;
	}

	//注意：在此之前必须执行release_config，否则会内存泄露！！！
	memset(cfg, 0, sizeof(user_config_set_t)*nr_config_array);

	//获取每个config set的数目，保存在cfg->user_config_set_t->nr_sets中
	while(fgets(buf,LINE_SIZE,fp) != NULL)
	{
		++line_nr;
		tmp = ltrim(buf);
		if(!tmp[0] || tmp[0] == '#')
			continue;

		if(tmp[0] == '[')
		{
			pitem = ++tmp;
			if( !(tmp = strchr(tmp, ']')) )
			{
				PRINTF("missing ] at line %d!\n", line_nr);
				continue;
			}
			*tmp = 0;

			i = -1;
			while(++i < nr_config_array)
			{
				if(strcmp(config_array[i].set_name, pitem) == 0 )
				{
					++(get_set_cfg(cfg, i)->nr_sets);
					break;
				}
			}
		}
		//continue;
	}

	//分配内存
	for(i=0; i<nr_config_array; ++i)
	{
		if( !(get_set_cfg(cfg, i)->cfgs = sp_alloc(config_array[i].set_size * get_set_cfg(cfg, i)->nr_sets, pool)) )
		{
			PRINTF("memory out in malloc config struct %d! %s\n", i, strerror(errno));
			while(--i > -1)
				free(get_set_cfg(cfg, i)->cfgs);
			return -1;
		}
		
		//置0的目的是为了方便后面读配置时跟踪配置项的位置
		get_set_cfg(cfg, i)->nr_sets = 0;
	}

	//设置默认配置
	if(default_cfg)
		default_cfg(cfg, pool);

	//逐个读入各组配置
	line_nr = 0;
	rewind(fp);
	//fseek(fp, 0, SEEK_SET);
	
	while(fgets(buf, LINE_SIZE, fp) != NULL)
	{
		++line_nr;
		tmp = ltrim(buf);
		if(!tmp[0] || tmp[0] == '#')
			continue;

		if(tmp[0] == '[')
		{
			pitem = ++tmp;
			if( !(tmp = strchr(tmp, ']')) )
			{
				PRINTF("missing ] at line %d!\n", line_nr);
				continue;
			}
			*tmp = 0;

			i = -1;
			while(++i < nr_config_array)
			{
				if(strcmp(config_array[i].set_name, pitem) == 0 )
				{
					current_set = &(config_array[i]);
					current_set_cfg = get_set_cfg(cfg, i);
					current_item_cfg = (char*)current_set_cfg->cfgs + current_set->set_size * current_set_cfg->nr_sets;
					++current_set_cfg->nr_sets;
					break;
				}
			}
			if(i == nr_config_array)
				PRINTF("unknown config set %s at line %d, %s may run error!\n", pitem, line_nr, __FUNCTION__);
			
			//下面的continue可以省略
			//continue;
		}
		else
		{
			pitem = tmp;
			if( !(tmp = strchr(buf, '=')) )
				continue;
			else
			{
				pvalue = tmp+1;
				*tmp = 0;
			}

			pitem = rtrim(pitem);
			pvalue = trim(pvalue);

			if(pvalue[0] == '"')	//字符串
			{
				tmp = ++pvalue;
				while(*tmp)
				{
					tmp = strchr(tmp, '"');
					if( *(tmp-1) != '\\')
					{
						*tmp = 0;
#ifdef _DEBUG
						i = 1;
#endif
						break;
					}
				}

#ifdef _DEBUG
				if(!i)
				{
					PRINTF("missing \"!\n");
					return -1;
				}
				else
					i = 0;
#endif
			}
			else
			{
				//去掉#之后的部分
				//if(! (tmp = strchr(pvalue, '#')) )
				//	tmp = strchr(pvalue, '\n');
				//if(tmp)	// 以防万一，怕最后没有换行，不然这个判断可以不要
				//	*tmp = 0;
				if( (tmp = my_strchr(pvalue, "#\n")) )
					*tmp = 0;
				pvalue = rtrim(pvalue);
			}

			i = -1;
			while(++i < current_set->nr_items)
			{
				if(strcmp(current_set->config_item_arr[i].item_name, pitem) == 0 )
				{
					if(	current_set->config_item_arr[i].config_reader(pvalue, 
						(char*)current_item_cfg + current_set->config_item_arr[i].offset, pool) < 0)
					{
						PRINTF("error: memory exhausted during reading config\n");
						return -1;
					}
					break;
				}
			}
		}

	}
			
	fclose(fp);    
	return 0;
}

void	release_config(config_t* config_array, int nr_config_array, void* cfg, M_stackpool* pool)
{
	int i = 0;
	user_config_set_t* set_cfg;

	for(i=0; i<nr_config_array; ++i)
	{
		set_cfg = get_set_cfg(cfg, i);
		if( set_cfg->cfgs )
		{
			sp_free(set_cfg->cfgs, pool);
			set_cfg->cfgs = NULL;
			set_cfg->nr_sets = 0;
		}
	}
}
