#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef WIN32
#include <arpa/inet.h>
#endif
#include "config.h"

#define LINE_SIZE 1024

//char* my_strtok(char* str, char* delim, char* buf);
//char* ltrim(char* str);
//char* rtrim(char* str);
//char* trim(char* str);

//ǿ��ת����i��ȫ�����ó�ԱΪuser_config_set_t����
#define get_set_cfg(cfg, i)						((user_config_set_t*)((char*)(cfg) + config_array[i].offset))

/*
	���α�������һ�λ�ø���set����Ŀ������ռ�
	�ڶ��β��������Ķ�������

	������Ҫ����ռ䣬���Ա�����release_config����
*/
int read_config(char *cfg_file, config_t* config_array, int nr_config_array, void* cfg)
{
	FILE *fp = NULL;
	char buf[LINE_SIZE]; //����������
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
		printf("open file %s fail, %s\n", cfg_file, strerror(errno));
		return -1;
	}

	//ע�⣺�ڴ�֮ǰ����ִ��release_config��������ڴ�й¶������
	memset(cfg, 0, sizeof(user_config_set_t)*nr_config_array);

	//��ȡÿ��config set����Ŀ��������cfg->user_config_set_t->nr_sets��
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
				printf("missing ] at line %d!\n", line_nr);
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
		continue;
	}

	//�����ڴ�
	for(i=0; i<nr_config_array; ++i)
	{
		if( !(get_set_cfg(cfg, i)->cfgs = malloc(config_array[i].set_size * get_set_cfg(cfg, i)->nr_sets)) )
		{
			printf("memory out in malloc config struct %d! %s\n", i, strerror(errno));
			while(--i > -1)
				free(get_set_cfg(cfg, i)->cfgs);
			return -1;
		}
		
		//��0��Ŀ����Ϊ�˷�����������ʱ�����������λ��
		get_set_cfg(cfg, i)->nr_sets = 0;
	}

	//��������������
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
				printf("missing ] at line %d!\n", line_nr);
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
				printf("unknown config set %s at line %d, %s may run error!\n", pitem, line_nr, __FUNCTION__);
			
			//�����continue����ʡ��
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

			if(pvalue[0] == '"')	//�ַ���
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
					printf("missing \"!\n");
					return -1;
				}
				else
					i = 0;
#endif
			}
			else
			{
				//ȥ��#֮��Ĳ���
				//if(! (tmp = strchr(pvalue, '#')) )
				//	tmp = strchr(pvalue, '\n');
				//if(tmp)	// �Է���һ�������û�л��У���Ȼ����жϿ��Բ�Ҫ
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
					current_set->config_item_arr[i].config_reader(pvalue, 
						(char*)current_item_cfg + current_set->config_item_arr[i].offset);
					break;
				}
			}
		}

	}
			
	fclose(fp);    
	return 0;
}

void	release_config(config_t* config_array, int nr_config_array, void* cfg)
{
	int i = 0;
	user_config_set_t* set_cfg;

	//�����ڴ�
	for(i=0; i<nr_config_array; ++i)
	{
		set_cfg = get_set_cfg(cfg, i);
		if( set_cfg->cfgs )
		{
			free(set_cfg->cfgs);
			set_cfg->cfgs = NULL;
			set_cfg->nr_sets = 0;
		}
	}
}