#include "normalize_engine_priv.h"
#include "normalize_engine.h"
#ifdef __M_CFG_OS_LINUX
#include <ctype.h>
#endif

#if (defined _WINDOWS) || (defined WIN32)
#ifdef _DEBUG
	#pragma comment(lib, "../debug/MBase.lib")
	#pragma comment(lib, "../debug/pthread.lib")
#else
	#pragma comment(lib, "../release/MBase.lib")
	#pragma comment(lib, "../release/pthread.lib")
#endif
#endif

config_set_t g_common_config[] = 
{
	{"flow_mode",				int8_reader,		offset_of(cfg_common_t, flow_mode)},
	{"group_numbers",			int8_reader,		offset_of(cfg_common_t, nr_groups)},
	{"pattern_id_len",			int8_reader,		offset_of(cfg_common_t, pattern_id_len)},
	{"group_delim_reuse",		int8_reader,		offset_of(cfg_common_t, group_delim_reuse)},
	{"escape_char",				char_reader,		offset_of(cfg_common_t, escape_char)},
	{"group_order",				string_reader,		offset_of(cfg_common_t, group_order)},
	{"default_group",			string_reader,		offset_of(cfg_common_t, default_group)},
	{"engine_size",				int_reader,			offset_of(cfg_common_t, engine_size)},
	{"handle_size",				int_reader,			offset_of(cfg_common_t, handle_size)},
	{"multi_seg_wildcard",		string_reader,		offset_of(cfg_common_t, wildcard) + FLAG_LEN*WT_MULTISEG},
	{"multi_char_wildcard",		string_reader,		offset_of(cfg_common_t, wildcard) + FLAG_LEN*WT_MULTICHAR},
	{"single_seg_wildcard",		string_reader,		offset_of(cfg_common_t, wildcard) + FLAG_LEN*WT_SINGLESEG},
	{"single_char_wildcard",	string_reader,		offset_of(cfg_common_t, wildcard) + FLAG_LEN*WT_SINGLECHAR}
};

config_set_t g_group_config[] = 
{
	{"name",			string_reader,		offset_of(cfg_group_t, name)},
	{"start_flag",		string_reader,		offset_of(cfg_group_t, start_flag)},
	{"end_flag",		string_reader,		offset_of(cfg_group_t, end_flag)},
	{"seg_delim",		string_reader,		offset_of(cfg_group_t, seg_delim)},
	{"seg_in_order",	int_reader,			offset_of(cfg_group_t, seg_in_order)}
};

config_set_t g_rule_config[] = 
{
	{"match_rule",		string_reader,		offset_of(cfg_rule_t, match_rule)},
	{"normal_rule",		string_reader,		offset_of(cfg_rule_t, normal_rule)}
};

config_t g_config[] = 
{
	{"common",	offset_of_cfg(ne_cfg_t, cfg_common_t),	sizeof(cfg_common_t),	g_common_config,	sizeof(g_common_config)/sizeof(config_set_t)},
	{"group",	offset_of_cfg(ne_cfg_t, cfg_group_t),	sizeof(cfg_group_t),	g_group_config,		sizeof(g_group_config)/sizeof(config_set_t)},
	{"rule",	offset_of_cfg(ne_cfg_t, cfg_rule_t),	sizeof(cfg_rule_t),		g_rule_config,		sizeof(g_rule_config)/sizeof(config_set_t)},

	//{"common",	offset_of(ne_cfg_t, common),	sizeof(cfg_common_t),	g_common_config,	sizeof(g_common_config)/sizeof(config_set_t)},
	//{"group",	offset_of(ne_cfg_t, group),		sizeof(cfg_group_t),	g_group_config,		sizeof(g_group_config)/sizeof(config_set_t)},
	//{"rule",	offset_of(ne_cfg_t, rule),		sizeof(cfg_rule_t),		g_rule_config,		sizeof(g_rule_config)/sizeof(config_set_t)},
};


static void set_default_config(ne_cfg_t* cfgs)
{
	sprintf(cfgs->cfg_common_t_cfgs->wildcard[WT_MULTICHAR], "*");
	sprintf(cfgs->cfg_common_t_cfgs->wildcard[WT_MULTISEG], "**");
	sprintf(cfgs->cfg_common_t_cfgs->wildcard[WT_SINGLECHAR], "$");
	sprintf(cfgs->cfg_common_t_cfgs->wildcard[WT_SINGLESEG], "$$");
	cfgs->cfg_common_t_cfgs->escape_char = '\\';
	cfgs->cfg_common_t_cfgs->pattern_id_len = 1;
	cfgs->cfg_common_t_cfgs->engine_size = 1024;
	cfgs->cfg_common_t_cfgs->handle_size = 1024;
}

M_sint32 read_ne_config(M_sint8* filename, ne_cfg_t* cfgs)
{
	return read_config(filename, g_config, sizeof(g_config)/sizeof(config_t), cfgs, set_default_config);
}

void release_ne_config(ne_cfg_t* cfgs)
{
	release_config(g_config, sizeof(g_config)/sizeof(config_t), cfgs);
}

void print_common_cfg(user_config_set_t* cfg)
{
	int i = 0;
	cfg_common_t* common = (cfg_common_t*)cfg->cfgs;
	for(i=0; i<cfg->nr_sets; i++)
	{
		printf("[common]\n");
		printf("default_group = %s\n", common->default_group);
		printf("group_order = %s\n", common->group_order);
		printf("nr_groups = %d\n", common->nr_groups);
		printf("pattern_id_len = %d\n", common->pattern_id_len);
		printf("escape_char = %c\n", common->escape_char);
		printf("multi_seg_wildcard = %s\n", common->wildcard[WT_MULTISEG]);
		printf("single_seg_wildcard = %s\n", common->wildcard[WT_SINGLESEG]);
		printf("multi_char_wildcard = %s\n", common->wildcard[WT_MULTICHAR]);
		printf("single_char_wildcard = %s\n\n", common->wildcard[WT_SINGLECHAR]);
		common++;
	}
}

void print_group_cfg(user_config_set_t* cfg)
{
	int i = 0;
	cfg_group_t* group = (cfg_group_t*)cfg->cfgs;
	for(i=0; i<cfg->nr_sets; i++)
	{
		printf("[group]\n");
		printf("start_flag = %s\n", group->start_flag);
		printf("end_flag = %s\n", group->end_flag);
		printf("name = %s\n", group->name);
		printf("seg_delim = %s\n", group->seg_delim);
		printf("seg_in_order = %d\n\n", group->seg_in_order);
		group++;
	}
}

void print_rule_cfg(user_config_set_t* cfg)
{
	int i = 0;
	cfg_rule_t* rule = (cfg_rule_t*)cfg->cfgs;
	for(i=0; i<cfg->nr_sets; i++)
	{
		printf("[rule]\n");
		printf("match_rule = %s\n", rule->match_rule);
		printf("normal_rule = %s\n\n", rule->normal_rule);
		rule++;
	}
}

void print_cfg(ne_cfg_t*	cfgs)
{
	//printf("%s\n", cfgs->cfg_common_t_cfgs[0].default_group);
	print_common_cfg(/*(user_config_set_t*)*/(&cfgs->cfg_common_t_cfgs));
	print_group_cfg(/*(user_config_set_t*)*/(&cfgs->cfg_group_t_cfgs));
	print_rule_cfg(/*(user_config_set_t*)*/(&cfgs->cfg_rule_t_cfgs));
}

/*
	build normalize engine
*/

typedef struct st_ne_arg
{
	M_stackpool	spool;		//alloc memory for persistent usage, such as radix tree
	M_stackpool tpool;		//alloc memory for temporary usage
	M_rt_pool	pat_pool;	//memory from tpool, used by pat_tree
	M_rt_arg	pat_arg;	//memory from tpool, used by pat_tree
	M_rt_pool	rm_pool;	//memory from spool, used by rm_tree
	M_rt_arg	rm_arg;		//memory from spool, used by rm_tree
	acmata_t	ac;			//temporary ac mata for delimiters, to split rules
	pattern_t*	wc_pat[NR_WILDCARD];	//temporary keep wildcard pattern here
							//if multi seg delim supported, only first seg delim is kept here
	M_rt_stub*	pat_tree;	//radix tree for pattern dedup
	M_sint32	nr_grps;
	
} ne_arg_t;

// keep delimiter matching result
typedef struct st_mat_delim
{
	M_dlist		delim_head;		//delim_pos_t->list_stub is used here
	M_dlist		wc_head;		//delim_pos_t->list_stub is also used here
	M_dlist		rest_delim;		//尚未完全匹配的delim
	M_dlist		rest_wc;		//尚未完全匹配的wildcard
	M_sint8*	ori_str;		//string that is processing
	M_sint32	ori_str_len;	//strlen of ori_str
	ne_arg_t*	ne_arg;
	M_sint32	nr_delim;		//number of delims
	M_sint16	nr_wc;			//number of wildcards
	M_sint8		escape_char;	//escape char
	M_sint8		leading_grp;	//group id of leading group
} mat_delim_t;


typedef struct st_str_dedup
{
	M_rt_stub	rt_stub;
	pattern_t*	pat;
} str_dedup_t;


// cmp type
#define CT_EQUAL	0
#define	CT_GREATER	1

// absorb type
#define AT_AFTER	0x04

// delim type
// DT_START, DT_END, DT_SEG are used for normal delim, can be combined together
// DT_HAT, DT_DOLLAR are used for special delim ^/$, can not be combined together
#define DT_START	0x01
#define DT_END		0x02
#define DT_SEG		0x04
#define DT_HAT		0x08
#define	DT_DOLLAR	0x10

// wildcard type
#define	WT_SINGLECHAR	0
#define	WT_SINGLESEG	1
#define	WT_MULTICHAR	2
#define	WT_MULTISEG		3

// pattern type，只有在执行输入串匹配时会遇到类型组合的情况，在构建engine时都是单类型
#define PT_STRING	0x1
#define	PT_DELIM	0x2
#define PT_WILDCARD	0x4

//// leading type of rm_wc_info_t
//#define	LT_WILDCARD	0
//#define	LT_DELIM	1

static INLINE void search_ac(ACSM_STRUCT2* acmata, M_sint8* str, M_sint32 str_len, void* data, int (*matcher)(void* id, void* tree, int index, void* data, void* neglist))
{
	M_sint32 state = 0;
	acsmSearch2(acmata, str, str_len, matcher, data, &state);
}

static INLINE M_sint8*	get_delim(M_sint8* str, M_sint8 c, M_sint8* buf, M_sint32* special_flag, M_sint16* str_len);

/*
	把各组按照定义的group order进行排序
*/
static INLINE void sort_cfg_group(ne_cfg_t* cfg, normalize_engine_t* model)
{
	cfg_group_t bak_grp;
	M_sint8	cur_grp[FLAG_LEN];
	M_sint8* grp_name;
	M_sint32 i,j;
	
	j = 0;
	grp_name = cfg->cfg_common_t_cfgs->group_order;
	while( (grp_name = get_delim(grp_name, ',', cur_grp, NULL, NULL)) )
	{
		for(i=j; i<cfg->cfg_common_t_cfgs->nr_groups; ++i)
		{
			if(!strcmp(cfg->cfg_group_t_cfgs[i].name, cur_grp))
				break;
		}

		if(i != j)
		{
			memcpy(&bak_grp, &cfg->cfg_group_t_cfgs[i], sizeof(cfg_group_t));
			memcpy(&cfg->cfg_group_t_cfgs[i], &cfg->cfg_group_t_cfgs[j], sizeof(cfg_group_t));
			memcpy(&cfg->cfg_group_t_cfgs[j], &bak_grp, sizeof(cfg_group_t));
		}
		++j;
	}

	for(i=0; i<cfg->cfg_group_t_nr_sets; ++i)
	{
		if(!strcmp(cfg->cfg_common_t_cfgs->default_group, cfg->cfg_group_t_cfgs[i].name))
		{
			model->default_grp = i;
			break;
		}
	}
}

static INLINE M_sint32 ne_model_init(normalize_engine_t* model, ne_arg_t* ne_arg, ne_cfg_t* cfg)
{
	//M_sint8		wc[sizeof(void*)] = {0};
	M_sint8		wc = 0;
	M_sint32	i = 0;

	if(!(model->acmata.ac_handle = acsmNew2(NULL, NULL, NULL)))
		return -1;
	if(!(model->tail_grps = (M_sint8*)sp_alloc(sizeof(M_sint8)*cfg->cfg_group_t_nr_sets, &ne_arg->spool)))
		return -1;
	if(!(model->head_grps = (M_sint8*)sp_alloc(sizeof(M_sint8)*cfg->cfg_group_t_nr_sets, &ne_arg->spool)))
		return -1;
	if(!(model->seg_pat = (pattern_t**)sp_alloc(sizeof(pattern_t*)*cfg->cfg_group_t_nr_sets, &ne_arg->spool)))
		return -1;


	model->rm_depth = 0;
	model->default_grp = -1;

	//rm_init_root(&model->rm_tree, sizeof(void*), wc);
	rm_init_root(&model->rm_tree, sizeof(M_sint8), &wc);
	memset(model->tail_grps, 0, sizeof(M_sint8)*cfg->cfg_group_t_nr_sets);
	memset(model->head_grps, 0, sizeof(M_sint8)*cfg->cfg_group_t_nr_sets);
	memset(model->seg_pat, 0, sizeof(pattern_t*)*cfg->cfg_group_t_nr_sets);

	model->disorder_seg = 0;
	for(i=0; i<cfg->cfg_group_t_nr_sets; ++i)
	{
		if(!cfg->cfg_group_t_cfgs[i].seg_in_order)
		{
			model->disorder_seg = 1;
			break;
		}
	}
	
	model->rules = NULL;
	model->nr_rules = 0;
	//model->nr_head_grps = 0;

	//memset(model->escape_chars, 0, sizeof(M_sint8)*256);
	
	model->memory = ne_arg->spool.pool;
	return 0;
}
static INLINE M_sint32 ne_arg_init(ne_arg_t* ne_arg, M_sint32 pool_size, M_sint32 nr_grps)
{
	void* mem = malloc(pool_size);
	void* mem2 = malloc(pool_size);

	if(!mem || !mem2)
	{
		if(mem)
			free(mem);
		if(mem2)
			free(mem2);
		return -1;
	}

	ne_arg->nr_grps = nr_grps;
	ne_arg->ac.ac_handle = acsmNew2(NULL, NULL, NULL);

	sp_init(mem, pool_size, &ne_arg->spool);
	sp_init(mem2, pool_size, &ne_arg->tpool);

	ne_arg->pat_tree = NULL;

	rt_init_pool(&ne_arg->pat_pool, (M_sint32)offset_of(str_dedup_t, rt_stub), 100);
	rt_pool_attach(&ne_arg->pat_pool, &ne_arg->tpool, sp_alloc, sp_free);

	ne_arg->pat_arg.dummy_node = ne_arg->pat_arg.extra_node = NULL;
	rt_process_arg(&ne_arg->pat_pool, &ne_arg->pat_arg);

	rm_init_pool(&ne_arg->rm_pool, 0, 100);
	rm_pool_attach(&ne_arg->rm_pool, &ne_arg->spool, sp_alloc, sp_free);

	ne_arg->rm_arg.dummy_node = ne_arg->rm_arg.extra_node = NULL;
	rm_process_arg(&ne_arg->rm_pool, &ne_arg->rm_arg);

	return 0;
}

static INLINE void	pat_init(pattern_t* pat)
{
	memset(pat, 0, sizeof(pattern_t));
	//slist_init(&pat->cons_head);
}

/*
	从配置文件的delim字段中解析出各个分隔符，需要处理字符转义问题
	取出的字符串从buf中返回
	special_flag: DT_HAT -- 得到特殊字符^
				  DT_DOLLAR -- 得到特殊字符$
				  could be NULL
	str_len: get length of string saved in buf, also could be NULL

	return的串如果为NULL，表示已经取完
	如果不为空，下次调用get_delim时传入的str即为上次的返回值
*/
static INLINE M_sint8*	get_delim(M_sint8* str, M_sint8 c, M_sint8* buf, M_sint32* special_flag, M_sint16* str_len)
{
	if(!*str)
		return NULL;

	if(*str == c)
		++str;

	if(special_flag)
		*special_flag = 0;
	if(str_len)
		*str_len = 0;
	while(*str && *str != c)
	{
		if(*str == '\\')
		{
			++str;
			switch(*str)
			{
			case '\\':
				*buf++ = '\\';
				break;
			case 'n':
				*buf++ = '\n';
				break;
			case 'r':
				*buf++ = '\r';
				break;
			case 't':
				*buf++ = '\t';
				break;
			default:	//其他情况沿用字符本意，包括,/$/^
				*buf++ = *str;
				break;
			}
		}
		else if(*str == '^')
		{
			assert(special_flag);
			*special_flag = DT_HAT;
			if(*(str+1) != c)
				printf("cfg file format error at %s\n", str);
		}
		else if(*str == '$')
		{
			assert(special_flag);
			*special_flag = DT_DOLLAR;
			if(*(str+1) != c)
				printf("cfg file format error at %s\n", str);
		}
		else
			*buf++ = *str;
		
		str++;
		if(str_len)
			++(*str_len);
	}

	*buf = 0;
	return str;
}

/*
	如果alloc_mem为真，分配一块小内存给delimiter用
*/
static INLINE pattern_t*	new_pat(ne_arg_t* ne_arg, M_sint32 alloc_mem)
{
	pattern_t*	pat;
	//if( !(pat = (pattern_t*)pi_alloc(sizeof(pattern_t), &ne_arg->pat_pool.valid_pool)) )
	if( !(pat = (pattern_t*)sp_alloc(sizeof(pattern_t), &ne_arg->spool)) )
		return NULL;

	pat_init(pat);

	if(alloc_mem)
	{
		if( !(pat->str = (M_sint8*)sp_alloc(sizeof(M_sint8)*FLAG_LEN, &ne_arg->spool)) )
		{
			//rt_free(&pat->rt_stub, &ne_arg->pat_pool);
			return NULL;
		}
	}
	return pat;
}

static INLINE void	delete_pat(pattern_t* pat, ne_arg_t* ne_arg, M_sint32 alloc_mem)
{
	if(alloc_mem && pat->str)
		sp_free(pat->str, &ne_arg->spool);
	if(pat->delim_type && pat->type == PT_DELIM)
		sp_free(pat->delim_type, &ne_arg->spool);

	sp_free(pat, &ne_arg->spool);
}

static INLINE pattern_t*	search_dedup_pat(ne_arg_t* ne_arg, pattern_t* pat)
{
	M_sint32 matched_len = 0;
	M_rt_stub* rt_stub = rt_search(ne_arg->pat_tree, pat->str, pat->str_len, RT_MODE_EXACT, &matched_len);

	if(rt_stub)
		return container_of(rt_stub, str_dedup_t, rt_stub)->pat;
	else
		return NULL;
}

/*
	解析配置文件中的分隔符串，构建delim_info_t，并将其插入到pat_tree和delim_set中

	delim_str：	配置文件中的串
	delim_type：分隔符类型，start/end/seg
	grp：		当前delim所属的group。同样的delim，同属于一个group的，在radix tree中只会出现一次
	model：		radix tree的所在
	ne_arg：	各种内存池
	i:			当前group的序号

	返回：0 成功 -1 失败
*/
static INLINE M_sint32	parse_group_delim_string(M_sint8* delim_str, M_sint16 delim_type, /*ne_cfg_t* cfg, */
	normalize_engine_t* model, ne_arg_t* ne_arg, M_sint32 i)
{
	M_rt_stub*		pat_stub;
	pattern_t*		pat;
	M_sint32		special_flag;
	M_sint32		alloc_size = sizeof(M_sint16)*ne_arg->nr_grps;
	M_sint32		alloc_mem = 1;
	str_dedup_t*	str_dedup;
	pattern_t*		dup_pat;

	if( !(pat = new_pat(ne_arg, alloc_mem)) )
		goto out;

	while( (delim_str = get_delim(delim_str, ',', pat->str, &special_flag, &pat->str_len)) )
	{
		switch(special_flag)
		{
		case DT_HAT:
		case DT_DOLLAR:
			assert(delim_type != DT_SEG);

			if(special_flag == DT_HAT)
			{
				assert(delim_type & DT_START);
				++model->head_grps[i];
			}
			else
			{
				assert(delim_type & DT_END);
				++model->tail_grps[i];
			}

			continue;

		default:
			pat->type = PT_DELIM;
			
			if( !(str_dedup = (str_dedup_t*)rt_alloc(sizeof(str_dedup_t), &ne_arg->pat_pool)) )
				return -1;
			rt_init_node(&str_dedup->rt_stub, pat->str, pat->str_len);
			str_dedup->pat = pat;

			dup_pat = NULL;
			rt_process_arg(&ne_arg->pat_pool, &ne_arg->pat_arg);
			if( !(pat_stub = rt_insert_node(&ne_arg->pat_tree, &str_dedup->rt_stub, &ne_arg->pat_arg)) )		// no dup
			{
				if( !(pat->delim_type = (M_sint16*)sp_alloc(alloc_size, &ne_arg->spool)) )
					goto out;
				memset(pat->delim_type, 0, alloc_size);
			}
			else
			{
				rt_free(&str_dedup->rt_stub, &ne_arg->pat_pool);
				dup_pat = pat;
				pat = container_of(pat_stub, str_dedup_t, rt_stub)->pat;
			}

			// till now, if duplicated, pat_stub is not NULL, else pat_stub is NULL
			pat->delim_type[i] |= delim_type;
			if(delim_type == DT_SEG && !model->seg_pat[i])
				model->seg_pat[i] = pat;

			if(dup_pat)
				pat = dup_pat;
			else
			{
				if( acsmAddPattern2(model->acmata.ac_handle, pat->str, pat->str_len, 1, 0, 0, 0, pat, 0) != 0 )
					goto out;
				if( acsmAddPattern2(ne_arg->ac.ac_handle, pat->str, pat->str_len, 1, 0, 0, 0, pat, 0) != 0 )
					goto out;
				if( !(pat = new_pat(ne_arg, alloc_mem)) )
					goto out;
			}
			break;
		}
	}
	delete_pat(pat, ne_arg, alloc_mem);
	return 0;

out:
	return -1;
}

static INLINE M_sint32	parse_group_delim(ne_cfg_t* cfg, normalize_engine_t* model, ne_arg_t* ne_arg, M_sint32 i)
{
	if(parse_group_delim_string(cfg->cfg_group_t_cfgs[i].start_flag, DT_START, /*cfg, */model, ne_arg, i) < 0)
		return -1;

	if(parse_group_delim_string(cfg->cfg_group_t_cfgs[i].end_flag, DT_END, /*cfg, */model, ne_arg, i) < 0)
		return -1;

	if(parse_group_delim_string(cfg->cfg_group_t_cfgs[i].seg_delim, DT_SEG, /*cfg, */model, ne_arg, i) < 0)
		return -1;

	return 0;
}

static INLINE M_sint32	parse_delims(normalize_engine_t* model, ne_cfg_t* cfg, ne_arg_t* ne_arg/*, pattern_t* seg_pat*/)
{
	M_sint32 i = 0;

	//将所有的delim加入到radix tree中
	for(i=0; i<cfg->cfg_group_t_nr_sets; ++i)
	{
		if(parse_group_delim(cfg, model, ne_arg, i) < 0)
			goto out;
		//if(!strcmp(cfg->cfg_common_t_cfgs->default_group, cfg->cfg_group_t_cfgs[i].name))
		//	model->default_grp = i;
	}
	return 0;

out:
	return -1;
}

/*
	检查delim是否满足约束：
	1. default group必需支持以^开头，以$结尾
	2. 如果有多个group支持以^开头，其end flag必不相同
*/
//static INLINE M_sint32	check_delims(normalize_engine_t* model, M_sint32 nr_grps)
//{
//	M_sint32 i, j;
//	if(!model->head_grps[model->default_grp] || !model->tail_grps[model->default_grp])
//		return -1;
//
//	for(i=0; i<nr_grps; ++i)
//	{
//		if(model->head_grps[i])
//		{
//			for(j=i; j<nr_grps; ++j)
//			{
//				if(model->head_grps[j])
//				{
//					//TODO..
//				}
//			}
//		}
//	}
//
//	return 0;
//}

static INLINE M_sint32 add_wildcard(ne_arg_t* ne_arg, pattern_t* wc_pat, M_sint8* wildcard, M_sint16 wc_type)
{
	wc_pat->type = PT_WILDCARD;
	//当类型为wildcard时，delim_type直接用来代表wildcard的type，不再另外分配空间
	wc_pat->delim_type = (M_sint16*)wc_type;
	wc_pat->str = wildcard;
	wc_pat->str_len = strlen(wildcard);

	if( acsmAddPattern2(ne_arg->ac.ac_handle, wc_pat->str, wc_pat->str_len, 1, 0, 0, 0, wc_pat, 0) != 0 )
		return -1;

	ne_arg->wc_pat[wc_type] = wc_pat;
	
	return 0;
}

static INLINE M_sint32 add_wildcards(ne_arg_t* ne_arg, pattern_t* wc_pat, ne_cfg_t* cfg)
{
	M_sint32 i = 0;
	for(i=0; i<NR_WILDCARD; ++i)
	{
		if(add_wildcard(ne_arg, &wc_pat[i], cfg->cfg_common_t_cfgs->wildcard[i], i) < 0)
			return -1;
	}

	return 0;
}

#define cmp_key_delim_tree	cmp_key_M_sint32
static INLINE void*		get_key_delim_tree(void* stub)
{
	return &(container_of(stub, delim_pos_t, rbt_stub)->pos);
}
static INLINE M_sint32	get_rbcolor_delim_tree(void* stub)
{
	return container_of(stub, delim_pos_t, rbt_stub)->color;
}
static INLINE void		set_rbcolor_delim_tree(void* stub, M_sint32 color)
{
	container_of(stub, delim_pos_t, rbt_stub)->color = color;
}

static INLINE void set_delim_pos(delim_pos_t* delim_pos, M_sint32 grp_id, M_sint32 seg_pos)
{
	delim_pos->grp_id = grp_id;
	delim_pos->seg_pos = seg_pos;
	//rbt_insert_node(root, &delim_pos->rbt_stub, 
	//	cmp_key_delim_tree, get_key_delim_tree, get_rbcolor_delim_tree, set_rbcolor_delim_tree);
}

static INLINE void refresh_delim_pos(delim_pos_t* head, delim_pos_t* tail, M_sint32 grp_id, M_sint32 seg_base)
{
	M_dlist* list_stub = &head->list_stub;
	delim_pos_t*	delim_pos;

	while(list_stub != &tail->list_stub)
	{
		delim_pos = container_of(list_stub, delim_pos_t, list_stub);
		list_stub = list_stub->next;

		if(delim_pos->delim_pat->delim_type[grp_id] & (DT_SEG | DT_START | DT_END))
			set_delim_pos(delim_pos, grp_id, seg_base++);
		else
			dlist_remove(NULL, &delim_pos->list_stub);
	}

	set_delim_pos(tail, grp_id, seg_base);
}

/*
	获得下一个可以为group head的delimiter，同时删除在这段路程中发现的所有delimiter
	为了兼容流模式要求，避免无休止的寻找，当发现的组分隔符的所有允许组号都小于当前组号时，返回失败
	组号从cur_grp开始，包括cur_grp
*/
static INLINE delim_pos_t* get_group_head(M_dlist* head, M_dlist* list_stub, M_sint32 cur_grp, M_sint32 nr_grps)
{
	delim_pos_t*	delim_pos;
	M_sint32		i;
	M_sint16		delim_type;
	M_sint32		find_group_delim = 0;

	if(++cur_grp == nr_grps)
		return NULL;

	while(list_stub != head)
	{
		delim_pos = container_of(list_stub, delim_pos_t, list_stub);
		list_stub = list_stub->next;

		for(i=0; i<nr_grps; ++i)
		{
			delim_type = delim_pos->delim_pat->delim_type[i];
			if(delim_type & DT_START) 
			{
				find_group_delim = 1;
				if(i >= cur_grp)
					return delim_pos;
			}
		}

		if(find_group_delim)
			return NULL;

		dlist_remove(head, &delim_pos->list_stub);
	}

	return NULL;
}
/*
	get group, seg info of an input string
	if success, return a tree 
*/
/*
	1. 把match result按照匹配位置顺序排列成一个链表，done
	2. 把delim_pos按照group进行组织，就是delim_set？需要么？pattern里面也有这些信息。。。，
		不过delim_set按group组织了，速度会快一些，done
	3. 对每个match_result的结果按照group查询（按顺序进行），最好能弄个状态机，但是实际上不太可能
		因为分隔符不确定。那就多次遍历吧，第一次遍历确定组边界，第二次遍历确定各段边界

	分割符的约束：
	1.	至少有一个group以^开头
	2.	允许多个group以^开头。但同以^开头的group必有不同的end delim，
		即能在第一时间确定第一组的类型。
	3.	必需要有默认组
	
	以上约束在解析配置文件时检查

	处理模式：
	1.	支持流式处理，即支持所有group都不以$结尾。一旦流式处理到了结尾，立刻认为回到^，直到流结束
	2.	允许同一个分隔符既是组头，又是段，又是组尾
	3.	按最长跨度进行分割
	4.	允许同类分隔符连续出现，等同一次的效果
	5.	如果一个输入串中没有任何分割字符，认为是默认组
*/
static INLINE M_sint32 split_string(mat_delim_t* match_delim, M_stackpool* spool, ne_cfg_t* cfg, normalize_engine_t* model, M_bst_stub** root)
{
	M_dlist*		delim_stub = match_delim->delim_head.next;
	M_dlist			*start_stub, *end_stub;
	delim_pos_t*	delim_pos;
	M_sint32		i;
	M_sint8*		candi_grps;
	M_sint32		nr_candi_grps = 0;
	M_sint32		nr_grps = cfg->cfg_group_t_nr_sets;
	M_sint16		delim_type;

	// 只为处理结束符后的通配符用，其他地方用不着
	M_dlist			*wc_stub, *wc_end;
	delim_pos_t*	wc_pos;

	/*
		head_pos：每组的起始位置
		last_candidate：上一个可以为组尾的候选位置
	*/
	delim_pos_t*	head_pos;// = container_of(delim_stub, delim_pos_t, list_stub);
	delim_pos_t*	last_candi_pos = NULL;
	M_sint32		last_candi_grp = -1;
	M_sint32		is_end, is_seg;
	M_sint32		final_grps;

	*root = NULL;

	if( dlist_empty(&match_delim->delim_head) )
	{
		match_delim->leading_grp = model->default_grp;
		return 0;
	}

	if( !(candi_grps = (M_sint8*)sp_alloc(sizeof(M_sint8)*nr_grps, spool)) )
		return -1;

	if(cfg->cfg_common_t_cfgs->flow_mode)	// 如果流模式
	{
		head_pos = get_group_head(&match_delim->delim_head, delim_stub, 0, nr_grps);
		for(i=0; i<nr_grps; ++i)
		{
			if(head_pos->delim_pat->delim_type[i] & DT_START)
				candi_grps[nr_candi_grps++] = i;
		}
	}
	else
	{
		head_pos = container_of(delim_stub, delim_pos_t, list_stub);
		for(i=0; i<nr_grps; ++i)
		{
			if(model->head_grps[i])
				candi_grps[nr_candi_grps++] = i;
		}
	}

	while(delim_stub != &match_delim->delim_head)
	{
		is_end = is_seg = final_grps = 0;
		delim_pos = container_of(delim_stub, delim_pos_t, list_stub);
		delim_stub = delim_stub->next;

		/* 
			由于所有组分隔符都是有效的，算法对每个组逐个检查分隔符，两种情况：
			1. end
				a. 且该end分隔符是某个候选组的seg ==> 更新对应end
				b. 且该end分隔符不是任意候选组的seg ==> 当前组匹配完成，取last candidate作为组end，需要检查下面的i
					i. 检查是否能够确定group，如果不能，失败退出
			2. !end	==> continue。算法只处理group分隔符，不处理seg分隔符
				
			确定了组和组的范围后，刷新组内的seg delim，删除那些不属于当前组的seg delim
		*/
		for(i=0; i<nr_candi_grps; ++i)
		{
			delim_type = delim_pos->delim_pat->delim_type[candi_grps[i]];

			//判断是否是有效seg
			if(delim_type & DT_SEG)
				is_seg = 1;

			if(delim_type & DT_END)
			{
				is_end = 1;
				last_candi_pos = delim_pos;
				last_candi_grp = candi_grps[i];
			}
		}

		if(!is_end || is_seg)				// 1.a, 2
			continue;

		for(i=0; i<nr_candi_grps; ++i)		// 检查1.b.i
		{
			delim_type = last_candi_pos->delim_pat->delim_type[candi_grps[i]];
			if(delim_type & DT_END)
			{
				last_candi_grp = candi_grps[i];
				//break;
				++final_grps;
			}
		}

		if(final_grps != 1)
			goto fail;

		if(&head_pos->list_stub == match_delim->delim_head.next && head_pos->pos > 0)
			refresh_delim_pos(head_pos, last_candi_pos, last_candi_grp, 1);
		else
			refresh_delim_pos(head_pos, last_candi_pos, last_candi_grp, 0);

		if(match_delim->leading_grp == -1)
			match_delim->leading_grp = last_candi_grp;

		// 重置循环变量
		if(cfg->cfg_common_t_cfgs->group_delim_reuse)	// 如果当前组尾同时还是下一组的组首
			head_pos = last_candi_pos;
		else
			head_pos = get_group_head(&match_delim->delim_head, last_candi_pos->list_stub.next, last_candi_grp + 1, nr_grps);

		nr_candi_grps = 0;
		if(head_pos)
		{
			delim_stub = head_pos->list_stub.next;
			for(i=last_candi_grp + 1; i<nr_grps; ++i)
			{
				if(head_pos->delim_pat->delim_type[i] & DT_START)
					candi_grps[nr_candi_grps++] = i;
			}
		}
		
		// 找不到head_pos，或遇到了某个只能为end，不能为start的分隔符：split结束，当前分隔符之后的内容丢弃
		// 流模式情况下要保留未能匹配的分隔符
		if(!nr_candi_grps)
		{
			match_delim->ori_str_len = last_candi_pos->pos;
			if(!cfg->cfg_common_t_cfgs->flow_mode)
				match_delim->ori_str[last_candi_pos->pos] = 0;
			start_stub = &last_candi_pos->list_stub;
			end_stub = match_delim->delim_head.prev;
			dlist_remove_list(&match_delim->delim_head, start_stub, end_stub);
			dlist_append_list(&match_delim->rest_delim, start_stub, end_stub);

			wc_stub = match_delim->wc_head.next;
			while(wc_stub != &match_delim->wc_head)
			{
				wc_pos = container_of(wc_stub, delim_pos_t, list_stub);
				if(wc_pos->pos > last_candi_pos->pos)
				{
					wc_end = match_delim->wc_head.prev;
					dlist_remove_list(&match_delim->wc_head, wc_stub, wc_end);
					dlist_append_list(&match_delim->rest_wc, wc_stub, wc_end);
					break;
				}
				wc_stub = wc_stub->next;
			}

			goto success;
		}

		last_candi_pos = NULL;
		last_candi_grp = -1;
	}

    if(match_delim->leading_grp == -1)
        match_delim->leading_grp = model->default_grp;

	// 非流模式下，所有delim都遍历完成，检查$分隔符
	if(!cfg->cfg_common_t_cfgs->flow_mode)
	{
		final_grps = 0;
		for(i=0; i<nr_candi_grps; ++i)
		{
			if(model->tail_grps[candi_grps[i]])
			{
				last_candi_grp = candi_grps[i];
				++final_grps;
			}
		}
		if(final_grps > 1)
			printf("warning: %d last candidate groups occur to string: %s\n", final_grps, match_delim->ori_str);
		refresh_delim_pos(head_pos, container_of(match_delim->delim_head.prev, delim_pos_t, list_stub), last_candi_grp, 0);
	}

	delim_stub = match_delim->delim_head.next;
	while(delim_stub != &match_delim->delim_head)
	{
		delim_pos = container_of(delim_stub, delim_pos_t, list_stub);
		delim_stub = delim_stub->next;
		rbt_insert_node(root, &delim_pos->rbt_stub, 
			cmp_key_delim_tree, get_key_delim_tree, get_rbcolor_delim_tree, set_rbcolor_delim_tree);
	}

success:
	sp_free(candi_grps, spool);
	return 0;
fail:
	sp_free(candi_grps, spool);
	return -1;
}

/*
	id:		pattern_t 
	data:	mat_res_t

	callback for ac mata.
	return value > 0: break ac processing
				 = 0: continue ac processing

	由于这个版本的AC不能区分大小写，所以得自己判断
	由于AC_FULL模式会出来很多相同的匹配结果，所以得自己过滤
	因为这两个原因导致这个回调变得很复杂
	此外还要检查有无转义
*/
static INLINE M_sint32	delim_matcher(pattern_t *id, void *tree, M_sint32 offset, mat_delim_t *data, void *neg_list)
{
	delim_pos_t* delim_pos;
	delim_pos_t* current;
	M_dlist*	list_stub;
	M_dlist*	list_head = &data->delim_head;
	M_sint32	i = offset - 1;

	//检查大小写是否匹配
	if( strncmp(id->str, data->ori_str + offset, id->str_len) )
		return 0;

	//对通配符检查是否有前导转义字符。由于存在出现连续转义字符的可能性，所以前导转义字符不得为奇数个
	while(data->ori_str[i] == data->escape_char)
		--i;
	if(!((offset - i) & 1))
		return 0;

	if( !(delim_pos = (delim_pos_t*)sp_alloc(sizeof(delim_pos_t), &data->ne_arg->tpool)))
		return 1;
	
	assert(offset >= 0);
	memset(delim_pos, 0, sizeof(delim_pos_t));
	//printf("matches %s, offset: %d, address: 0x%p\n", id->str, offset, id);

	delim_pos->pos = offset;
	delim_pos->delim_pat = id;

	dlist_append(list_head, &delim_pos->list_stub);
	
	current = delim_pos;

	list_stub = delim_pos->list_stub.prev;
	while(list_stub != list_head)
	{
		delim_pos = container_of(list_stub, delim_pos_t, list_stub);
		list_stub = list_stub->prev;

		//如果之前的匹配结果的offset比当前匹配结果的offset大，
		//表示有更长的匹配串，删除之前的匹配结果
		if(delim_pos->pos >= offset)	
			dlist_remove(list_head, &delim_pos->list_stub);
		else if(delim_pos->pos + delim_pos->delim_pat->str_len > offset)
		{
			//如果之前的匹配结果加上模式串长度大于当前的offset，表示有交叉发生，删除当前的匹配结果
			dlist_remove(list_head, &current->list_stub);
			break;
		}
		else
			break;
	}
	return 0;
}

/*
	将通配符匹配结果从delim_head中取出来，放到wc_head中
*/
static INLINE void	process_delim_matcher(mat_delim_t *mat_delim)
{
	M_dlist*	list_head = &mat_delim->delim_head;
	M_dlist*	list_stub  = list_head->next;
	delim_pos_t*	delim_pos;

	while(list_stub != list_head)
	{
		delim_pos = container_of(list_stub, delim_pos_t, list_stub);
		list_stub = list_stub->next;
		++mat_delim->nr_delim;

		if(delim_pos->delim_pat->type == PT_WILDCARD)
		{
			dlist_remove(list_head, &delim_pos->list_stub);
			dlist_append(&mat_delim->wc_head, &delim_pos->list_stub);
			++mat_delim->nr_wc;
			--mat_delim->nr_delim;
		}
	}
}

static INLINE void	process_escape_char(mat_delim_t* match_delim)
{
	M_sint32		i = 0;
	M_sint32		j = 0;
	M_sint8*		str = match_delim->ori_str;
	M_sint32		str_len = match_delim->ori_str_len;
	M_dlist*		list_stub = match_delim->delim_head.next;
	delim_pos_t*	delim_pos = container_of(list_stub, delim_pos_t, list_stub);

	while(i < str_len)
	{
		if(str[i] == match_delim->escape_char)
			str[j] = str[++i];
		str[++j] = str[++i];
		
		if(delim_pos && delim_pos->pos == i)
		{
			delim_pos->pos -= (i-j);
			list_stub = list_stub->next;
			delim_pos = container_of(list_stub, delim_pos_t, list_stub);
		}
	}

	str[j] = 0;
	match_delim->ori_str_len = j;
}

/*
	记录一个以delim_pos开头的segment
*/
typedef struct st_string_seg
{
	delim_pos_t*	delim_pos;
	M_sint8*		str;
	M_sint32		str_len;
} string_seg_t;

static INLINE M_sint32 cmp_delim_pos(string_seg_t* x1, string_seg_t* x2)
{
	M_sint32 x1_len = x1->str_len - x1->delim_pos->delim_pat->str_len;
	M_sint32 x2_len = x2->str_len - x2->delim_pos->delim_pat->str_len;
	M_sint32 len = M_min(x1_len, x2_len);
	M_sint32 i = 0;
	M_sint8* str1 = x1->str + x1->delim_pos->delim_pat->str_len;
	M_sint8* str2 = x2->str + x2->delim_pos->delim_pat->str_len;
	
	while(i < len && *str1 == *str2)
	{
		++i;
		++str1;
		++str2;
	}

	if(i<len)
		return *str1 - *str2;
	else
	{
		if(x1->str_len == x2->str_len)
			return 0;
		else
			return x1_len > x2_len ? 1 : -1;
	}
}

/*
	为每个wc_pos设置group id和seg pos

	并利用wc_pos中的rbt_stub的left和right域标记当前wc_pos两侧的delimiter
*/
static INLINE void	set_wc_pos(mat_delim_t* match_delim)
{
	M_dlist* delim_stub = match_delim->delim_head.next;
	M_dlist* wc_stub = match_delim->wc_head.next;
	delim_pos_t	*delim_pos, *wc_pos;
	M_sint32	last_grp, last_seg_pos;

	match_delim->nr_delim = match_delim->nr_wc = 0;
	last_grp = match_delim->leading_grp;
	last_seg_pos = 0;
	delim_pos = NULL;

	while(delim_stub != &match_delim->delim_head)
	{
		delim_pos = container_of(delim_stub, delim_pos_t, list_stub);
		delim_stub = delim_stub->next;
		++match_delim->nr_delim;

		while(wc_stub != &match_delim->wc_head)
		{
			wc_pos = container_of(wc_stub, delim_pos_t, list_stub);
			if(wc_pos->pos < delim_pos->pos)
			{
				wc_pos->seg_pos = last_seg_pos;
				wc_pos->grp_id = last_grp;

				wc_pos->rbt_stub.right = delim_pos;
				if(delim_pos->list_stub.prev != &match_delim->delim_head)
					wc_pos->rbt_stub.left = container_of(delim_pos->list_stub.prev, delim_pos_t, list_stub);
				else
					wc_pos->rbt_stub.left = NULL;

				wc_stub = wc_stub->next;
				++match_delim->nr_wc;
			}
			else
				break;
		}

		last_grp = delim_pos->grp_id;
		last_seg_pos = delim_pos->seg_pos;
	}

	while(wc_stub != &match_delim->wc_head)
	{
		wc_pos = container_of(wc_stub, delim_pos_t, list_stub);
		wc_pos->seg_pos = last_seg_pos;
		wc_pos->grp_id = last_grp;
		
		wc_pos->rbt_stub.right = NULL;
		if(!delim_pos)
			wc_pos->rbt_stub.left = NULL;
		else
			wc_pos->rbt_stub.left = delim_pos;

		wc_stub = wc_stub->next;
		++match_delim->nr_wc;
	}
}

/*
	刷新指定delim pos list中的pos参量，返回第一个不属于当前grp的list stub

	返回NULL表示到了链表尾
*/
static INLINE M_dlist*	update_x_pos(M_dlist* list_head, M_dlist* list_stub, M_sint8 grp, M_sint32 pos_shift, M_bst_stub** root)
{
	delim_pos_t* x_pos;
	if(list_stub)
	{
		while(list_stub != list_head)
		{
			x_pos = container_of(list_stub, delim_pos_t, list_stub);
			if(x_pos->grp_id > grp)
				return list_stub;
			
			if(root)
			{
				rbt_remove_node(root, &x_pos->rbt_stub, get_rbcolor_delim_tree, set_rbcolor_delim_tree);
				x_pos->pos += pos_shift;
				rbt_insert_node(root, &x_pos->rbt_stub, cmp_key_delim_tree, get_key_delim_tree, get_rbcolor_delim_tree, set_rbcolor_delim_tree);
			}
			else
				x_pos->pos += pos_shift;

			list_stub = list_stub->next;
		}
	}

	return NULL;
}

static INLINE delim_pos_t*	create_dummy_delim(M_sint8 grp, M_sint32 nr_grps, M_stackpool* tpool)
{
	delim_pos_t* dummy_delim;

	if( !(dummy_delim = (delim_pos_t*)sp_alloc(sizeof(delim_pos_t), tpool)) )
		return NULL;
	if( !(dummy_delim->delim_pat = (pattern_t*)sp_alloc(sizeof(pattern_t), tpool)) )
		return NULL;
	if( !(dummy_delim->delim_pat->delim_type = (M_sint16*)sp_alloc(sizeof(M_sint16)*nr_grps, tpool)) )
		return NULL;

	memset(dummy_delim->delim_pat->delim_type, 0, sizeof(M_sint16)*nr_grps);

	dummy_delim->pos = 0;
	dummy_delim->grp_id = grp;
	dummy_delim->seg_pos = 0;
		
	dummy_delim->delim_pat->str = NULL;
	dummy_delim->delim_pat->str_len = 0;
	dummy_delim->delim_pat->type = PT_DELIM;
	memset(dummy_delim->delim_pat->delim_type, 0, sizeof(M_sint16)*nr_grps);
	dummy_delim->delim_pat->delim_type[grp] = DT_START;

	return dummy_delim;
}

/* 
	重排无序组的分隔符和通配符结构
	1. 每个seg由其leading delim_pos 唯一代表，通过delim_pos->pos，以及match_delim->ori_str获得
	2. 从链表和树中取下所有的delim pos和wc pos，并将wc_pos按seg挂在对应delim pos的list_stub中，
	   修改pos为相对该delim pos的偏移
	3. 返回0表示能找到属于该组的delim，所有的wc都挂在delim下，而delim挂在delim_head下
	   返回1表示找不到属于该组的delim，所有wc都挂在delim_head下。只有首段能出现这种情况

	如果某段没有leading delim_pos，但是有wc_pos，则创建一个dummy delim_pos，挂在delim_head下返回
	保持数据结构一致。后续处理时需要区分dummy delim_pos。
	事实上，只有非流模式的首段会出现这种情况，代码里用assert验证
	返回下一段的group id

	4. 检查每个delim pos的wc_pos，看看是否包含段通配，如果有，该delim pos无效，去掉
	5. 最后得到一个delim pos的数组，对其排序，并重编delim pos链表和wc_pos链表，注意修改各自的pos域
*/
static INLINE M_sint32	rearrange_delim_pos(mat_delim_t* match_delim, M_dlist* delim_head, M_dlist* delim_stub, 
	M_dlist* wc_stub, M_sint8 grp, M_bst_stub** root, M_sint32* end_pos)
{
	delim_pos_t	*delim_pos, *wc_pos;
	M_sint32	has_delim = 0;
	M_dlist*	wc_head;
	M_sint32	next_grp = match_delim->ne_arg->nr_grps;
	delim_pos_t* dummy_delim;
	
	*end_pos = match_delim->ori_str_len;
	while(delim_stub != &match_delim->delim_head)
	{
		delim_pos = container_of(delim_stub, delim_pos_t, list_stub);
		delim_stub = delim_stub->next;
		if(delim_pos->grp_id > grp)
		{
			*end_pos = delim_pos->pos;
			next_grp = delim_pos->grp_id;
			break;
		}

		if(!has_delim && delim_pos->seg_pos)	// 起始段不是组首，创建dummy delim
		{
			assert(delim_head->next == delim_head && grp == match_delim->leading_grp);
			if( !(dummy_delim = create_dummy_delim(grp, match_delim->ne_arg->nr_grps, &match_delim->ne_arg->tpool)) )
				return -1;
			delim_pos = dummy_delim;
			delim_stub = delim_stub->prev;
		}
		else
		{
			dlist_remove(&match_delim->delim_head, &delim_pos->list_stub);
			rbt_remove_node(root, &delim_pos->rbt_stub, get_rbcolor_delim_tree, set_rbcolor_delim_tree);
		}

		dlist_append(delim_head, &delim_pos->list_stub);
		dlist_init((M_dlist*)&delim_pos->rbt_stub);

		has_delim = 1;

		while(wc_stub != &match_delim->wc_head)
		{
			wc_pos = container_of(wc_stub, delim_pos_t, list_stub);

			if(wc_pos->seg_pos == delim_pos->seg_pos && wc_pos->grp_id == delim_pos->grp_id)
			{
				wc_stub = wc_stub->next;

				dlist_remove(&match_delim->wc_head, &wc_pos->list_stub);
				wc_pos->pos -= delim_pos->pos;
				dlist_append((M_dlist*)&delim_pos->rbt_stub, &wc_pos->list_stub);
			}
			else
				break;
		}
	}

	if(!has_delim)
	{
		assert(delim_head->next == delim_head && grp == match_delim->leading_grp);
		if( !(dummy_delim = create_dummy_delim(grp, match_delim->ne_arg->nr_grps, &match_delim->ne_arg->tpool)) )
			return -1;
		dlist_append(delim_head, &dummy_delim->list_stub);
		dlist_init((M_dlist*)&delim_pos->rbt_stub);
		delim_pos = dummy_delim;
		//has_delim = 1;
	}

	// 还有通配没有处理完
	if(wc_stub != &match_delim->wc_head)
	{
		wc_pos = container_of(wc_stub, delim_pos_t, list_stub);
		wc_head = (M_dlist*)&delim_pos->rbt_stub;
        dlist_init(wc_head);
		while(wc_stub != &match_delim->wc_head && wc_pos->grp_id == grp)
		{
			dlist_remove(&match_delim->wc_head, &wc_pos->list_stub);
			wc_pos->pos -= delim_pos->pos;
			dlist_append(wc_head, wc_stub);

			wc_stub = wc_stub->next;
			wc_pos = container_of(wc_stub, delim_pos_t, list_stub);
		}
	}

	return next_grp;
}

static INLINE M_dlist*	insert_delim(M_dlist* leading_stub, delim_pos_t* insert_delim_pos, 
	M_sint8* dst_buf, M_sint32* dst_pos, M_sint8 grp, M_sint32 seg_pos, M_bst_stub** root)
{
	insert_delim_pos->pos = *dst_pos;
	insert_delim_pos->grp_id = grp;
	insert_delim_pos->seg_pos = seg_pos;

	dlist_insert(leading_stub, &insert_delim_pos->list_stub);
	if(root)
		rbt_insert_node(root, &insert_delim_pos->rbt_stub, cmp_key_delim_tree, get_key_delim_tree, get_rbcolor_delim_tree, set_rbcolor_delim_tree);

	memcpy(dst_buf + *dst_pos, insert_delim_pos->delim_pat->str, insert_delim_pos->delim_pat->str_len);
	*dst_pos += insert_delim_pos->delim_pat->str_len;

	return &insert_delim_pos->list_stub;
}

/*
	插入一个完整的由leading delim pos标志的seg。
	
	所有delim pos都已经摘下来了，且该seg的通配都挂在delim_pos->rbt_stub为首的链表中
	
	需要做的事：
	1. 拷贝串到目的buffer：dst_buf @ dst_pos
	2. 将delim pos加入到match_delim的链表，以及root树中，修改pos为目的buffer的pos
	3. 将wc pos加入到match_delim的wc链表中，修改pos为目的buffer的pos

	初始时delim_pos的pos相对原串，wc_pos的pos相对当前段

	这个过程不修改pos_shift，返回当前插入的最后一个通配的list_stub，作为下一次的leading_wc_stub
	之所以返回通配而不是分隔符的位置，是因为分隔符每次只插入一个，而通配会插入不确定的数目
*/
static INLINE M_dlist*	insert_seg(M_dlist*	leading_delim_stub, string_seg_t* string_seg, pattern_t* replace_pat, M_dlist* leading_wc_stub, 
	mat_delim_t* match_delim, M_sint8* dst_buf, M_sint32* dst_pos, M_sint8 grp, M_sint32 seg_pos, M_bst_stub** root)
{
	//M_sint32		seg_len;
	delim_pos_t*	insert_delim_pos = string_seg->delim_pos;
	delim_pos_t		*wc_pos;
	M_dlist			*wc_stub;
	M_dlist*		wc_list = (M_dlist*)&insert_delim_pos->rbt_stub;
	M_dlist			*wc_start, *wc_end;
	
	memcpy(dst_buf + *dst_pos + replace_pat->str_len, 
		match_delim->ori_str + insert_delim_pos->pos + insert_delim_pos->delim_pat->str_len,
		string_seg->str_len);
	
	// 修改delim_pos的pos，但不加入到树中，因为wc还没处理
	insert_delim_pos->delim_pat = replace_pat;
	if(insert_delim_pos->delim_pat->str)	// not dummy delim
		insert_delim(leading_delim_stub, insert_delim_pos, dst_buf, dst_pos, grp, seg_pos, NULL);
	*dst_pos += string_seg->str_len;
	
	wc_stub = wc_list->next;
	while(wc_stub != wc_list)
	{
		wc_pos = container_of(wc_stub, delim_pos_t, list_stub);
		wc_stub = wc_stub->next;
		wc_pos->grp_id = grp;
		wc_pos->seg_pos = seg_pos;
		wc_pos->pos += insert_delim_pos->pos;
	}

	wc_start = wc_list->next;
	wc_end = wc_list->prev;
	if(wc_start != wc_list)
	{
		dlist_remove_list(wc_list, wc_start, wc_end);
		dlist_insert_list(leading_wc_stub, wc_start, wc_end);
	}
	
	if(root && insert_delim_pos->delim_pat->str)
	{
		rbt_insert_node(root, &insert_delim_pos->rbt_stub, 
			cmp_key_delim_tree, get_key_delim_tree, get_rbcolor_delim_tree, set_rbcolor_delim_tree);
	}

	return (wc_start == wc_list ? leading_wc_stub : wc_end);
}

/*
	插入一个段通配，返回wc list中的当前插入节点
*/
static INLINE M_dlist*	insert_multiseg_wc(M_dlist* leading_wc_stub, mat_delim_t* match_delim, M_sint8* dst_buf, 
	M_sint32* dst_pos, M_sint8 grp, M_sint32 seg_pos)
{
	delim_pos_t* wc_pos = (delim_pos_t*)sp_alloc(sizeof(delim_pos_t), &match_delim->ne_arg->tpool);

	if(!wc_pos)
		return NULL;

	wc_pos->delim_pat = match_delim->ne_arg->wc_pat[WT_MULTISEG];
	wc_pos->pos = *dst_pos;
	wc_pos->grp_id = grp;
	wc_pos->seg_pos = seg_pos;
	dlist_insert(leading_wc_stub, &wc_pos->list_stub);

	memcpy(dst_buf + *dst_pos, wc_pos->delim_pat->str, wc_pos->delim_pat->str_len);
	*dst_pos += wc_pos->delim_pat->str_len;
	
	return &wc_pos->list_stub;
}

static INLINE M_sint32	is_group_start(mat_delim_t* match_delim, delim_pos_t* delim_pos)
{
	if(delim_pos->list_stub.prev == &match_delim->delim_head)
		return (match_delim->leading_grp == delim_pos->grp_id) ? 0 : 1;
	else
		return (container_of(delim_pos->list_stub.prev, delim_pos_t, list_stub)->grp_id == delim_pos->grp_id) ? 0 : 1;
}

/*
	当seg可以disorder的时候，要做几件事：
	1. 去掉这一组的所有段通配
	2. 将这一组剩下的通配取出，改绝对偏移为相对段首偏移，并与段首绑定
	3. 对剩下的段排序
	4. 构造新组，在排序后的每两段中插入**通配段，并在组首和组尾都插入**通配段
	5. 刷新通配的偏移

	对于in order的seg，同样也要修改其内部通配的绝对偏移，当它前面有可以disorder的group时。

	所以在process segorder的时候，必需处理所有分隔符和通配符的绝对偏移
*/
static INLINE M_sint32 process_rule_segorder(mat_delim_t* match_delim, ne_cfg_t* cfg, normalize_engine_t* model, M_bst_stub** root)
{
	M_sint8*	buf;
	M_sint32	i = match_delim->leading_grp, j, k;
	M_sint32	start_pos, end_pos, dst_pos;
	M_sint32	pos_shift = 0;

	M_dlist		*delim_stub, *wc_stub, *bak_delim_stub, *bak_wc_stub;
	delim_pos_t	*delim_pos, *wc_pos, *head_delim_pos, *last_delim_pos, *insert_delim_pos;

	string_seg_t*	delim_pos_arr;	// 从match_delim中取出分隔符后筛选有效的放在这个数组中
	M_sint32		nr_pos;			// delim_pos_arr数组的有效长度
	M_dlist			delim_head;
	pattern_t		*head_pat, *replace_pat;
	M_sint32		seg_pos;
	M_sint32		next_grp;
	M_sint32		is_start;		// 标记head_delim_pos的类型

	if(!model->disorder_seg)
		return 0;
	if( !(buf = (M_sint8*)sp_alloc(match_delim->ori_str_len << 1, &match_delim->ne_arg->spool)) )
		return -1;
	if( !(delim_pos_arr = (string_seg_t*)sp_alloc(sizeof(string_seg_t)*(match_delim->nr_delim+1), &match_delim->ne_arg->tpool)) )
		return -1;

	// 遍历所有的通配，为其分配group id和seg_pos
	set_wc_pos(match_delim);

	dst_pos = start_pos = 0;
	delim_stub = match_delim->delim_head.next;
	wc_stub = match_delim->wc_head.next;
	while(i<cfg->cfg_group_t_nr_sets)
	{
		// 如果当前组有序，那么只要遍历一遍，顺便把delim和wc的位置平移一下即可
		if(cfg->cfg_group_t_cfgs[i].seg_in_order)
		{
			wc_stub = update_x_pos(&match_delim->wc_head, wc_stub, i, pos_shift, NULL);
			delim_stub = update_x_pos(&match_delim->delim_head, delim_stub, i, pos_shift, root);
			if(delim_stub)
			{
				delim_pos = container_of(delim_stub, delim_pos_t, list_stub);
				i = delim_pos->grp_id;
				end_pos = delim_pos->pos;
			}
			else
			{
				i = cfg->cfg_group_t_nr_sets;
				end_pos = match_delim->ori_str_len;
			}

			memcpy(buf + dst_pos, match_delim->ori_str + start_pos, end_pos - start_pos);
			dst_pos += end_pos - start_pos;
		}
		else
		{
			dlist_init(&delim_head);
			nr_pos = 0;
			is_start = 0;
			bak_delim_stub = delim_stub->prev;
			bak_wc_stub = wc_stub->prev;
			
			if( (next_grp = rearrange_delim_pos(match_delim, &delim_head, delim_stub, wc_stub, i, root, &end_pos)) < 0)
				return -1;
			
			//备份起始分隔符信息。因为有了dummy_delim，heat_delim_pos一定在组首（有可能是dummy delim）
			delim_stub = delim_head.next;
			head_delim_pos = container_of(delim_stub, delim_pos_t, list_stub);
			head_pat = head_delim_pos->delim_pat;
			if(is_group_start(match_delim, head_delim_pos))
				is_start = 1;
			
			// 检查是否有段通配。将没有段通配的段加入到delim_pos_arr中
			while(delim_stub != &delim_head)
			{
				delim_pos = container_of(delim_stub, delim_pos_t, list_stub);
				delim_stub = delim_stub->next;
				delim_pos->color = 0;
				
				wc_stub = ((M_dlist*)&delim_pos->rbt_stub)->next;
				while(wc_stub != (M_dlist*)&delim_pos->rbt_stub)
				{
					wc_pos = container_of(wc_stub, delim_pos_t, list_stub);
					wc_stub = wc_stub->next;
					if((M_sint16)wc_pos->delim_pat->delim_type == WT_MULTISEG ||
						(M_sint16)wc_pos->delim_pat->delim_type == WT_SINGLESEG)
					{
						delim_pos->color = 1;
						break;
					}
				}

				// 先全部加到数组
				if(nr_pos > 0)
				{
					last_delim_pos = delim_pos_arr[nr_pos-1].delim_pos;
					delim_pos_arr[nr_pos-1].str_len = delim_pos->pos - last_delim_pos->pos 
						- last_delim_pos->delim_pat->str_len;
				}
				delim_pos_arr[nr_pos].delim_pos = delim_pos;
				delim_pos_arr[nr_pos].str = match_delim->ori_str + delim_pos->pos;
				++nr_pos;
			}

			last_delim_pos = delim_pos_arr[nr_pos-1].delim_pos;
			delim_pos_arr[nr_pos-1].str_len = end_pos - last_delim_pos->pos 
				- last_delim_pos->delim_pat->str_len;

			j = k = 0;
			while(j < nr_pos)
			{
				if(delim_pos_arr[j].delim_pos->color)
					++j;
				else
					memcpy(&delim_pos_arr[k++], &delim_pos_arr[j++], sizeof(string_seg_t));
			}
			nr_pos = k;

			// 排序
			if(nr_pos > 1)
				qsort(delim_pos_arr, nr_pos, sizeof(string_seg_t), cmp_delim_pos);

			// 重构数据结构
			seg_pos = 0;
			if(is_start)
			{
				if( !(insert_delim_pos = (delim_pos_t*)sp_alloc(sizeof(delim_pos_t), &match_delim->ne_arg->tpool)) )
					return -1;
				insert_delim_pos->delim_pat = head_pat;
				if( !(bak_delim_stub = insert_delim(bak_delim_stub, insert_delim_pos, buf, &dst_pos, i, seg_pos, root)) )
					return -1;
			}

			if( !(bak_wc_stub = insert_multiseg_wc(bak_wc_stub, match_delim, buf, &dst_pos, i, seg_pos)) )
				return -1;
			++seg_pos;

			for(j=0; j<nr_pos; ++j)
			{
				// 借用head_pat
				if(delim_pos_arr[j].delim_pos->delim_pat == head_pat)
					replace_pat = model->seg_pat[i];
				else
					replace_pat = delim_pos_arr[j].delim_pos->delim_pat;

				// 插入seg
				if( !(bak_wc_stub = insert_seg(bak_delim_stub, &delim_pos_arr[j], replace_pat, bak_wc_stub, match_delim,
					buf, &dst_pos, i, seg_pos, root)) )
					return -1;
				bak_delim_stub = &delim_pos_arr[j].delim_pos->list_stub;
				++seg_pos;

				// 插入分隔符和段通配。。
				if( !(insert_delim_pos = (delim_pos_t*)sp_alloc(sizeof(delim_pos_t), &match_delim->ne_arg->tpool)) )
					return -1;
				insert_delim_pos->delim_pat = model->seg_pat[i];
				if( !(bak_delim_stub = insert_delim(bak_delim_stub, insert_delim_pos, buf, &dst_pos, i, seg_pos, root)) )
					return -1;
				if( !(bak_wc_stub = insert_multiseg_wc(bak_wc_stub, match_delim, buf, &dst_pos, i, seg_pos)) )
					return -1;
				++seg_pos;
			}

			pos_shift = dst_pos - end_pos;
			delim_stub = bak_delim_stub->next;
			wc_stub = bak_wc_stub->next;
			i = next_grp;
			
		}
		start_pos = end_pos;
	}
	sp_free(delim_pos_arr, &match_delim->ne_arg->tpool);
	
	match_delim->ori_str = buf;
	match_delim->ori_str_len = dst_pos;
	match_delim->ori_str[dst_pos] = 0;

	return 0;
}

//static INLINE void print_mata_info(pattern_t** pat_arr, M_sint32 nr_pats, rule_t* rule)
//{
//	M_sint32 i = 0;
//	M_sint8	 buf[1024];
//	M_sint32 pos = 0;
//
//	buf[0] = 0;
//	
//	for(i=0; i<nr_pats; ++i)
//	{
//		if(pat_arr[i])
//		{
//			M_snprintf(buf + pos, pat_arr[i]->str_len + 1, "%s", pat_arr[i]->str);
//			pos += pat_arr[i]->str_len;
//		}
//		else
//		{
//			M_snprintf(buf + pos, 3, "**");
//			pos += 2;
//		}	
//	}
//
//	printf("%s\n", buf);
//}

static INLINE void print_char_mata_info(M_sint8* buf, M_sint32 str_len)
{
	M_sint32 pos = 0;
	M_sint32 i = 0;

	while(i < str_len)
	{
		pos = printf("%s", buf);
		buf += pos;
		i += pos;
		if(i < str_len)
		{
			printf("*");
			++buf;
			++i;
		}
	}

	printf("\n");
}

static INLINE void	rule_init(rule_t* rule, M_sint8* match_rule, M_sint8* normal_rule)
{
	memset(rule, 0, sizeof(rule_t));
	rule->match_rule = match_rule;
	rule->normal_rule = normal_rule;
	rule->match_rule_len = strlen(rule->match_rule);
	rule->normal_rule_len = strlen(rule->normal_rule);
	slist_init(&rule->nm_head);
}

//返回-1表示该通配无编号
static INLINE M_sint32 get_wc_seq(mat_delim_t* mat_delim, delim_pos_t* wc_pos, ne_cfg_t* cfg)
{
	M_sint32 seq_len = cfg->cfg_common_t_cfgs->pattern_id_len;
	M_sint8	 tmp[8];
	M_sint32 i = 0;
	M_sint8* seq = mat_delim->ori_str + wc_pos->pos + wc_pos->delim_pat->str_len;
	assert(seq_len > 0);

	for(i=0; i<seq_len; ++i)
	{
		tmp[i] = *seq++;
		if(!isdigit(tmp[i]))
			return -1;
	}
	tmp[i] = 0;
	return atoi(tmp);
}

static INLINE void	set_normal_info(normal_info_t* normal_info, M_sint8* str, M_sint32 str_len, M_sint32 wc_seq, M_sint8 wc_type, rule_t* rule)
{
	M_sint32	i;
	wc_info_t*	wc_info = rule->ori_wc_arr;
	normal_info->str = str;
	normal_info->str_len = str_len;
	normal_info->next_wc = NULL;
	slist_insert(&rule->nm_head, &normal_info->nm_stub);

	for(i=0; i<rule->nr_ori_wc; ++i)
	{
		if(wc_info->wc_seq == wc_seq && wc_info->wc_type == wc_type)
		{
			normal_info->next_wc = wc_info;
			return;
		}
		++wc_info;
	}

	return;
}

static INLINE M_sint32 process_normal_rule(mat_delim_t* match_delim, ne_cfg_t* cfg, normalize_engine_t* model, rule_t* rule)
{
	mat_delim_t	match_delim_normal;

	M_dlist*		wc_stub;
	delim_pos_t*	wc_pos;
	normal_info_t*	normal_info;
	M_sint32		wc_seq;
	M_sint32		start_pos = 0;

	dlist_init(&match_delim_normal.delim_head);
	dlist_init(&match_delim_normal.wc_head);

	match_delim_normal.ne_arg = match_delim->ne_arg;
	match_delim_normal.escape_char = match_delim->escape_char;
	
	match_delim_normal.ori_str = rule->normal_rule;
	match_delim_normal.ori_str_len = strlen(match_delim_normal.ori_str);
	match_delim_normal.nr_wc = 0;

	// 下面的初始化操作对normal rule无意义，normal rule不split
	//match_delim_normal.nr_delim = 0;
	//match_delim_normal.leading_grp = -1;
	//dlist_init(&match_delim_normal.rest_delim);
	//dlist_init(&match_delim_normal.rest_wc);

	search_ac(match_delim_normal.ne_arg->ac.ac_handle, match_delim_normal.ori_str, match_delim_normal.ori_str_len, &match_delim_normal, delim_matcher);
	//acsmSearch2(match_delim_normal.ne_arg->ac.ac_handle, match_delim_normal.ori_str, match_delim_normal.ori_str_len, delim_matcher, &match_delim_normal, &state);

	// 处理match_rule中的转义字符，只有在处理规则串时需要做这一步
	// 这一步会修改原str的内容，以及match_delim中各个pos成员，并将修改后的长度从str_len返回
	process_escape_char(&match_delim_normal);

	// 将匹配结果中的通配符拿到match_delim->wc_head中。只有在处理规则串时需要做这一步，处理一般输入串不需要这个步骤
	process_delim_matcher(&match_delim_normal);

	wc_stub = match_delim_normal.wc_head.next;
	while(wc_stub != &match_delim_normal.wc_head)
	{
		wc_pos = container_of(wc_stub, delim_pos_t, list_stub);

		// 检查通配是否有编号，如果无编号，该规则不合法
		if( (wc_seq = get_wc_seq(&match_delim_normal, wc_pos, cfg)) < 0 )
			return -1;
		if( !(normal_info = (normal_info_t*)sp_alloc(sizeof(normal_info_t), &match_delim_normal.ne_arg->spool)) )
			return -1;
		if(!wc_pos->pos)	// normal rule以通配开头
			set_normal_info(normal_info, NULL, 0, wc_seq, (M_sint8)wc_pos->delim_pat->delim_type, rule);
		else
			set_normal_info(normal_info, match_delim_normal.ori_str + start_pos, wc_pos->pos - start_pos, wc_seq, (M_sint8)wc_pos->delim_pat->delim_type, rule);

		if(!normal_info->next_wc)
			return -1;
		start_pos = wc_pos->pos + wc_pos->delim_pat->str_len + cfg->cfg_common_t_cfgs->pattern_id_len;
		wc_stub = wc_stub->next;
	}

	if(start_pos != match_delim_normal.ori_str_len)
	{
		if( !(normal_info = (normal_info_t*)sp_alloc(sizeof(normal_info_t), &match_delim_normal.ne_arg->spool)) )
			return -1;
		set_normal_info(normal_info, match_delim_normal.ori_str + start_pos, match_delim_normal.ori_str_len - start_pos, -1, -1, rule);
	}

	slist_reverse(&rule->nm_head);
	
	return 0;
}

static INLINE M_sint32	get_rm_wc_info_start_pos(rm_wc_info_t* rm_wc_info)
{
	if(rm_wc_info->nr_delims)
		return M_min(rm_wc_info->delim_info[0].pos, rm_wc_info->wc_info[0].pos);
	else
		return rm_wc_info->wc_info[0].pos;
}

static INLINE M_sint32	get_rm_wc_info_end_pos(rm_wc_info_t* rm_wc_info)
{
	delim_info_t* delim_info;
	wc_info_t* wc_info = &rm_wc_info->wc_info[rm_wc_info->nr_wcs - 1];
	if(rm_wc_info->nr_delims)
	{
		delim_info = &rm_wc_info->delim_info[rm_wc_info->nr_delims - 1];
		return delim_info->pos > wc_info->pos ?	
			delim_info->pos + delim_info->delim_pat->str_len : wc_info->pos + wc_info->wc_len;
	}
	else
		return wc_info->pos + wc_info->wc_len;
}

/*
	这里只能用wc_pos作seg pos检查，因为wc_info中的seg pos考虑了段通配影响
*/
static INLINE M_sint32	is_connected(wc_info_t* wc_info1, delim_pos_t* delim_pos, wc_info_t* wc_info2, delim_pos_t* wc_pos1, delim_pos_t* wc_pos2)
{
	if(wc_info1->pos + wc_info1->wc_len == wc_info2->pos)
	{
		if(wc_pos1->grp_id != wc_pos2->grp_id || wc_pos1->seg_pos != wc_pos2->seg_pos)
			return -1;
		if(wc_info1->wc_type == WT_MULTISEG || wc_info1->wc_type == WT_SINGLESEG)
			return -1;
		if(wc_info2->wc_type == WT_MULTISEG || wc_info2->wc_type == WT_SINGLESEG)
			return -1;
		return 1;
	}
	else if(delim_pos && wc_info1->pos + wc_info1->wc_len == delim_pos->pos &&
		delim_pos->pos + delim_pos->delim_pat->str_len == wc_info2->pos)
	{
		if(wc_pos1->grp_id == wc_pos2->grp_id)
		{
			if(wc_pos1->seg_pos + 1 != wc_pos2->seg_pos || delim_pos->grp_id != wc_pos2->grp_id
				|| delim_pos->seg_pos != wc_pos2->seg_pos)	
				return -1;
		}
		else
		{
			if(wc_pos2->seg_pos || delim_pos->seg_pos)
				return -1;
		}

		if(wc_info1->wc_type == WT_MULTISEG || wc_info2->wc_type == WT_MULTISEG)
			return 1;
	}

	return 0;
}

static INLINE M_sint32	complete_rm_wc_info(rm_wc_info_t* rm_wc_info, mat_delim_t* match_delim)
{
	M_dlist* list_stub = &((delim_pos_t*)rm_wc_info->delim_info)->list_stub;
	delim_pos_t* delim_pos;
	M_sint32 i = 0;
	delim_info_t* delim_info;

	if(!rm_wc_info->delim_info)
	{
		rm_wc_info->nr_delims = 0;
		return 0;
	}

	while(list_stub != &match_delim->delim_head)
	{
		delim_pos = container_of(list_stub, delim_pos_t, list_stub);
		list_stub = list_stub->next;

		if(delim_pos->pos + delim_pos->delim_pat->str_len <= rm_wc_info->nr_delims)
			++i;
		else
			break;
	}

	list_stub = &((delim_pos_t*)rm_wc_info->delim_info)->list_stub;
	rm_wc_info->nr_delims = i;
	if( !(rm_wc_info->delim_info = sp_alloc(i*sizeof(delim_info_t), &match_delim->ne_arg->spool)) )
		return -1;

	i = 0;
	delim_info = rm_wc_info->delim_info;
	while(list_stub != &match_delim->delim_head)
	{
		delim_pos = container_of(list_stub, delim_pos_t, list_stub);
		list_stub = list_stub->next;

		if(i < rm_wc_info->nr_delims)
		{
			delim_info->delim_pat = delim_pos->delim_pat;
			delim_info->grp_id = delim_pos->grp_id;
			delim_info->pos = delim_pos->pos;
			delim_info->seg_pos = delim_pos->seg_pos;
			++delim_info;
			++i;
		}
		else
			break;
	}

	return 0;
}

/*
	设置wc_info的所有内容，包括合并通配

	返回0表示没有合并发生
	返回1表示当前wildcard和之前的wildcard发生了合并
	当之前的wildcard发生了合并，下一个进行set wc info的wildcard就不需要合并
	这个信息通过is_merged参数传入
	参数中的i是当前wc_info的下标，nr_wcs则是所有wc_info的个数
*/
static INLINE rm_wc_info_t*	set_wc_info(wc_info_t* wc_info, delim_pos_t* wc_pos, normalize_engine_t* model, mat_delim_t* match_delim, ne_cfg_t* cfg, 
	rm_wc_info_t*	last_rm_wc_info, /*M_sint32 is_merged, */rule_t* rule)
{
	delim_pos_t		*left, *right;
	M_sint32 ret;

	memset(wc_info, 0, sizeof(wc_info_t));
	wc_info->wc_type = (M_sint8)(wc_pos->delim_pat->delim_type);
	wc_info->pos = wc_pos->pos;
	wc_info->wc_pat = wc_pos->delim_pat;
	wc_info->grp = wc_pos->grp_id;
	wc_info->seg_pos = wc_pos->seg_pos;

	left = wc_pos->rbt_stub.left;
	right = wc_pos->rbt_stub.right;

	if( (wc_info->wc_seq = get_wc_seq(match_delim, wc_pos, cfg)) < 0)
		wc_info->wc_len = wc_info->wc_pat->str_len;
	else
		wc_info->wc_len = wc_info->wc_pat->str_len + cfg->cfg_common_t_cfgs->pattern_id_len;
	
	// 检查段通配是否跨越了整段
	if(wc_info->wc_type == WT_SINGLESEG || wc_info->wc_type == WT_MULTISEG)
	{
		if(!left)
		{
			if(wc_info->pos) return NULL;
		}
		else
		{
			if(wc_info->pos != left->pos + left->delim_pat->str_len)	return NULL;
		}
		if(!right)
		{
			if(wc_info->pos + wc_info->wc_len != match_delim->ori_str_len)		return NULL;
		}
		else
		{
			if(wc_info->pos + wc_info->wc_len != right->pos)			return NULL;
		}
	}

	// last_rm_wc_info代表当前wc_info将要合入的rm_wc_info
	if(!last_rm_wc_info)
	{
		last_rm_wc_info = rule->rm_wc_arr;
		last_rm_wc_info->wc_info = wc_info;
		++rule->nr_rm_wc;
		assert(rule->ori_wc_arr == wc_info);
	}
	else
	{
		if( (ret = is_connected(wc_info-1, left, wc_info, 
			container_of(wc_pos->list_stub.prev, delim_pos_t, list_stub), wc_pos)) < 0 )
			return NULL;

		if(!ret)
		{
			// 为之前的rm_wc_info扫尾。。TODO
			assert((&rule->rm_wc_arr[rule->nr_rm_wc-1].wc_info[rule->rm_wc_arr[rule->nr_rm_wc-1].nr_wcs -1])+1 == wc_info);
			if( complete_rm_wc_info(last_rm_wc_info, match_delim) < 0 )
				return NULL;
			last_rm_wc_info = &rule->rm_wc_arr[rule->nr_rm_wc++];
			last_rm_wc_info->wc_info = wc_info;

		}
	}

	/*
		这里的逻辑是：
		1. 尽量多的往last_rm_wc_info中加入相连的分隔符
		2. 只有当前通配为**通配时才加入两端的delim；否则只加入通配自己

		由于需要到收尾时才创建数组，所以这里只是用delim_info域记下来delim_pos_t的起始位置
		以及用nr_delims成员暂时保存当前last_rm_wc_info的尾部
	*/
	++last_rm_wc_info->nr_wcs;
	wc_info->rm_wc_info = last_rm_wc_info;

	// 这段代码只吸收 ** 通配两端的分隔符
	if(wc_info->wc_type == WT_MULTISEG)
	{
		if(!last_rm_wc_info->delim_info || last_rm_wc_info->nr_delims != left->pos + left->delim_pat->str_len)
			last_rm_wc_info->delim_info = left ? left : right;
		last_rm_wc_info->nr_delims = right ? right->pos + right->delim_pat->str_len : wc_info->pos + wc_info->wc_len;
	}
	else
		last_rm_wc_info->nr_delims = wc_info->pos + wc_info->wc_len;

	// 这段代码吸收所有通配两端的分隔符，和上面的代码互斥
	//if(left && left->pos + left->delim_pat->str_len == wc_info->pos)
	//{
	//	if(!last_rm_wc_info->delim_info)
	//		last_rm_wc_info->delim_info = left;
	//}
	//if(last_rm_wc_info->nr_delims < wc_info->pos + wc_info->wc_len)
	//	last_rm_wc_info->nr_delims = wc_info->pos + wc_info->wc_len;

	//if(right && right->pos == wc_info->pos + wc_info->wc_len)
	//{
	//	if(!last_rm_wc_info->delim_info)
	//		last_rm_wc_info->delim_info = right;
	//	if(last_rm_wc_info->nr_delims < right->pos + right->delim_pat->str_len)
	//		last_rm_wc_info->nr_delims = right->pos + right->delim_pat->str_len;
	//}

	return last_rm_wc_info;
}

/*
	检查所有通配是否合法，并创建对应的wc_info_t对象。一旦发现不合法的通配符，失败返回
*/
static INLINE M_sint32 process_wildcard(mat_delim_t* match_delim, ne_cfg_t* cfg, normalize_engine_t* model, rule_t* rule)
{
	M_dlist*		wc_stub = match_delim->wc_head.next;
	delim_pos_t*	wc_pos;
	wc_info_t*		wc_info;
	rm_wc_info_t*	last_rm_wc_info = NULL;
	M_sint32		nr_multiseg_wc = 0;	// number of multi seg wildcards
	//M_sint32		is_merged = 1;		// 初始设为1是为了避免第一次就进行merge检测

	set_wc_pos(match_delim);
	rule->nr_ori_wc = match_delim->nr_wc;

	if(!match_delim->nr_wc)
		return 0;
	if( !(rule->ori_wc_arr = (wc_info_t*)sp_alloc(sizeof(wc_info_t)*match_delim->nr_wc, &match_delim->ne_arg->spool)) )
		return -1;
	if( !(rule->rm_wc_arr = (rm_wc_info_t*)sp_alloc(sizeof(rm_wc_info_t)*match_delim->nr_wc, &match_delim->ne_arg->spool)) )
		return -1;
	wc_info = rule->ori_wc_arr;
	memset(rule->rm_wc_arr, 0, sizeof(rm_wc_info_t)*match_delim->nr_wc);	// 初始化一下，set_wc_info要用
	
	while(wc_stub != &match_delim->wc_head)
	{
		wc_pos = container_of(wc_stub, delim_pos_t, list_stub);
		wc_stub = wc_stub->next;
		
		if(wc_pos->grp_id != (wc_info-1)->grp)
			nr_multiseg_wc = 0;

		if( !(last_rm_wc_info = set_wc_info(wc_info, wc_pos, model, match_delim, cfg, last_rm_wc_info, rule)) )
			return -1;
		
		if(nr_multiseg_wc)
		{
			wc_info->cmp_type = CT_GREATER;
			wc_info->seg_pos -= nr_multiseg_wc;
		}
		else
			wc_info->cmp_type = CT_EQUAL;

		if(wc_info->wc_type == WT_MULTISEG)
		{
			++nr_multiseg_wc;
			wc_info->cmp_type = CT_GREATER;	// 因为multiseg可能反向匹配，所以只能用greater
		}

		++wc_info;
	}

	// 处理最后一个last_rm_wc_info
	if( complete_rm_wc_info(last_rm_wc_info, match_delim) < 0 )
		return -1;

	return 0;
}

///*
//	要避免待插入的串已经是分隔符，这种串要组合类型。。
//*/
//static INLINE M_sint32	create_ac_string_pat(pattern_t** pat, ne_arg_t* ne_arg, rule_t* rule, 
//	normalize_engine_t* model, M_sint32 start_pos, M_sint32 end_pos)
//{
//	str_dedup_t*	str_dedup;
//	pattern_t*		dup_pat = NULL;
//	M_rt_stub*		pat_stub;
//	
//	if( !(*pat = new_pat(ne_arg, 0)) )
//		return -1;
//	if( !(str_dedup = (str_dedup_t*)rt_alloc(sizeof(str_dedup_t), &ne_arg->pat_pool)) )
//		return -1;
//
//	(*pat)->str = rule->match_rule + start_pos;
//	(*pat)->str_len = end_pos - start_pos;
//	(*pat)->type = PT_STRING;
//
//	rt_init_node(&str_dedup->rt_stub, (*pat)->str, (*pat)->str_len);
//	str_dedup->pat = *pat;
//
//	rt_process_arg(&ne_arg->pat_pool, &ne_arg->pat_arg);
//	if( (pat_stub = rt_insert_node(&ne_arg->pat_tree, &str_dedup->rt_stub, &ne_arg->pat_arg)) )		// dup
//	{
//		rt_free(&str_dedup->rt_stub, &ne_arg->pat_pool);
//		delete_pat(*pat, ne_arg, 0);
//
//		*pat = container_of(pat_stub, str_dedup_t, rt_stub)->pat;
//		(*pat)->type |= PT_STRING;
//	}
//	else
//	{
//		if( acsmAddPattern2(model->acmata.ac_handle, (*pat)->str, (*pat)->str_len, 1, 0, 0, 0, *pat, 0) != 0 )
//			return -1;
//	}
//
//	return 0;
//}
//
///*
//	创建AC和radix mata一起完成，因为这二者之间关系密切，都要基于rm_wc_info
//*/
//static INLINE M_sint32 create_ac_and_radix_mata(normalize_engine_t* model, rule_t* rule, ne_arg_t* ne_arg)
//{
//	rm_wc_info_t*	rm_wc_info = rule->rm_wc_arr;
//	wc_info_t*		wc_info;
//	pattern_t**		pat_arr;		// NULL表示通配，非空表示pat
//	M_sint32		nr_pats = 0;
//	M_sint32		i;
//	M_sint32		start_pos = 0;
//	M_sint32		end_pos = 0;
//	M_rm_stub*		rm_stub;
//
//	if( !(pat_arr = (pattern_t**)sp_alloc(sizeof(pattern_t*) * (rule->nr_rm_wc << 1) + 1, &ne_arg->spool)) )
//		return -1;
//
//	if(!rule->nr_rm_wc)		// 没有通配，整个规则加入
//	{
//		if( create_ac_string_pat(&pat_arr[nr_pats++], ne_arg, rule, model, start_pos, strlen(rule->match_rule)) < 0 )
//			return -1;
//	}
//	else
//	{
//		// wc没有顶头，处理wc之前的情况
//		if(rm_wc_info->wc_info[0].pos)
//		{
//			if( create_ac_string_pat(&pat_arr[nr_pats++], ne_arg, rule, model, start_pos, rm_wc_info->wc_info[0].pos) < 0 )
//				return -1;
//			//wc_info = rm_wc_info->wc_info2 ? rm_wc_info->wc_info2 : rm_wc_info->wc_info1;
//			//start_pos = get_rm_wc_info_end_pos(rm_wc_info);//wc_info->pos + wc_info->wc_len;
//		}
//
//		// 处理每个wc之后的情况，不包括最后一个wc
//		for(i=0; i<rule->nr_rm_wc - 1; ++i)
//		{
//			pat_arr[nr_pats++] = NULL;
//			start_pos = get_rm_wc_info_end_pos(rm_wc_info);
//			end_pos = get_rm_wc_info_start_pos(++rm_wc_info);
//
//			if( create_ac_string_pat(&pat_arr[nr_pats++], ne_arg, rule, model, start_pos, end_pos) < 0 )
//				return -1;
//			//wc_info = rm_wc_info->wc_info2 ? rm_wc_info->wc_info2 : rm_wc_info->wc_info1;
//			//start_pos = wc_info->pos + wc_info->wc_len;
//			//start_pos = get_rm_wc_info_end_pos(rm_wc_info);
//		}
//
//		// 处理最后一个wc
//		start_pos = get_rm_wc_info_end_pos(rm_wc_info);
//		pat_arr[nr_pats++] = NULL;
//		if(start_pos < strlen(rule->match_rule))
//		{
//			if( create_ac_string_pat(&pat_arr[nr_pats++], ne_arg, rule, model, start_pos, strlen(rule->match_rule)) < 0 )
//				return -1;
//		}
//	}
//
//	if( !(rm_stub = rm_alloc(sizeof(M_rm_stub), &ne_arg->rm_pool)) )
//		return -1;
//	rm_init_node(&model->rm_tree, rm_stub, pat_arr, nr_pats);
//	rm_process_arg(&ne_arg->rm_pool, &ne_arg->rm_arg);
//	rm_insert_node(&model->rm_tree, rm_stub, &ne_arg->rm_arg, rule);
//
//	if(nr_pats > model->rm_depth)
//		model->rm_depth = nr_pats;
//
//	print_mata_info(pat_arr, nr_pats, rule);
//
//	return 0;
//}

static INLINE M_sint32 insert_radix_mata(M_sint8* str, M_sint32 str_len, normalize_engine_t* model, rule_t* rule, ne_arg_t* ne_arg)
{
	M_rm_stub*		rm_stub;

	if( !(rm_stub = (M_rm_stub*)rm_alloc(sizeof(M_rm_stub), &ne_arg->rm_pool)) )
		return -1;
	rm_init_node(&model->rm_tree, rm_stub, str, str_len);
	rm_process_arg(&ne_arg->rm_pool, &ne_arg->rm_arg);
	rm_insert_node((M_rmt_root*)&model->rm_tree, rm_stub, &ne_arg->rm_arg, rule);

	return 0;
}

static INLINE M_sint32 create_radix_mata(normalize_engine_t* model, rule_t* rule, ne_arg_t* ne_arg)
{
	rm_wc_info_t*	rm_wc_info = rule->rm_wc_arr;
	M_sint8*		buf;
	M_sint32		i;
	M_sint32		str_len = rule->match_rule_len;
	M_sint32		pos = 0;
	M_sint32		start_pos = 0;
	M_sint32		end_pos = 0;

	if( !(buf = (M_sint8*)sp_alloc(sizeof(M_sint8) * (str_len + 1), &ne_arg->spool)) )
		return -1;

	if(!rule->nr_rm_wc)		// 没有通配，整个规则加入
	{
		memcpy(buf, rule->match_rule, str_len);
		if( insert_radix_mata(buf, str_len, model, rule, ne_arg) < 0 )
			return -1;

		pos = str_len;
	}
	else
	{
		// wc没有顶头，处理wc之前的情况
		if(rm_wc_info->wc_info[0].pos)
		{
			end_pos = get_rm_wc_info_start_pos(rm_wc_info);
			memcpy(buf + pos, rule->match_rule, end_pos);
			pos += end_pos;
		}

		// 处理每个wc之后的情况，不包括最后一个wc
		for(i=0; i<rule->nr_rm_wc - 1; ++i)
		{
			buf[pos++] = 0;
			start_pos = get_rm_wc_info_end_pos(rm_wc_info);
			end_pos = get_rm_wc_info_start_pos(++rm_wc_info);

			memcpy(buf + pos, rule->match_rule + start_pos, end_pos - start_pos);
			pos += end_pos - start_pos;
		}

		// 处理最后一个wc
		start_pos = get_rm_wc_info_end_pos(rm_wc_info);
		buf[pos++] = 0;
		if(start_pos < str_len)
		{
			memcpy(buf + pos, rule->match_rule + start_pos, str_len - start_pos);
			pos += str_len - start_pos;
		}
	}

	buf[pos] = 0;
	if( insert_radix_mata(buf, pos, model, rule, ne_arg) < 0 )
		return -1;

#ifdef _DEBUG_PRINT
	print_char_mata_info(buf, pos);
#endif

	return 0;
}

static INLINE void print_delim_match_result(mat_delim_t* match_delim, ne_cfg_t* cfg)
{
	delim_pos_t* delim_pos;
	M_dlist* pos_stub = match_delim->delim_head.next;

	while(pos_stub != &match_delim->delim_head)
	{
		delim_pos = container_of(pos_stub, delim_pos_t, list_stub);
		printf("%s, offset: %d, seg pos: %d, address: 0x%p, belongs group %s as %d\n", 
			delim_pos->delim_pat->str, delim_pos->pos, delim_pos->seg_pos, delim_pos->delim_pat, 
			cfg->cfg_group_t_cfgs[delim_pos->grp_id].name, delim_pos->delim_pat->delim_type[delim_pos->grp_id] );
		pos_stub = pos_stub->next;
	}

	pos_stub = match_delim->wc_head.next;
	while(pos_stub != &match_delim->wc_head)
	{
		delim_pos = container_of(pos_stub, delim_pos_t, list_stub);
		printf("%s, offset: %d, id: %d, address: 0x%p\n", delim_pos->delim_pat->str, delim_pos->pos, 
			get_wc_seq(match_delim, delim_pos, cfg), delim_pos);
		pos_stub = pos_stub->next;
	}
}

static INLINE void print_normal_rule(rule_t* rule)
{
	M_sint8			buf[1024];
	M_slist*		nm_stub = rule->nm_head.next;
	normal_info_t*	nm_node;
	M_sint32		cur_pos = 0;

	while(nm_stub != &rule->nm_head)
	{
		nm_node = container_of(nm_stub, normal_info_t, nm_stub);
		nm_stub = nm_stub->next;

		if(nm_node->str_len)
		{
			M_snprintf(buf + cur_pos, nm_node->str_len + 1, "%s", nm_node->str);
			cur_pos += nm_node->str_len;
			M_snprintf(buf + cur_pos, 3, "%s", "->");
			cur_pos += 2;
		}

		if(nm_node->next_wc)
		{
			M_snprintf(buf + cur_pos, nm_node->next_wc->wc_len + 3, "%s%d%s", nm_node->next_wc->wc_pat->str, nm_node->next_wc->wc_seq, "->");
			cur_pos += nm_node->next_wc->wc_len + 2;
		}
	}

	printf("%s\n", buf);
}

static INLINE M_sint32	parse_rules(normalize_engine_t* model, ne_cfg_t* cfg, ne_arg_t* ne_arg)
{
	M_sint32	i = 0;
	M_sint32	state = 0;
	mat_delim_t	match_delim;
	M_bst_stub*	root;
	rule_t*		rule;


	match_delim.ne_arg = ne_arg;
	match_delim.escape_char = cfg->cfg_common_t_cfgs->escape_char;

	if( !(model->rules = (rule_t*)sp_alloc(sizeof(rule_t)*cfg->cfg_rule_t_nr_sets, &ne_arg->spool)) )
		return -1;

	rule = model->rules;
	
	for(i=0; i<cfg->cfg_rule_t_nr_sets; i++)
	{
		dlist_init(&match_delim.delim_head);
		dlist_init(&match_delim.wc_head);
		dlist_init(&match_delim.rest_delim);
		dlist_init(&match_delim.rest_wc);
		match_delim.ori_str = cfg->cfg_rule_t_cfgs[i].match_rule;
		match_delim.ori_str_len = strlen(match_delim.ori_str);
		match_delim.nr_wc = 0;
		match_delim.nr_delim = 0;
		match_delim.leading_grp = -1;
		//acsmSearch2(ne_arg->ac.ac_handle, match_delim.ori_str, match_delim.ori_str_len, delim_matcher, &match_delim, &state);
		search_ac(ne_arg->ac.ac_handle, match_delim.ori_str, match_delim.ori_str_len, &match_delim, delim_matcher);

		// 处理match_rule中的转义字符，只有在处理规则串时需要做这一步
		// 这一步会修改原str的内容，以及match_delim中各个pos成员，并将修改后的长度从str_len返回
		process_escape_char(&match_delim);

		// 将匹配结果中的通配符拿到match_delim->wc_head中。只有在处理规则串时需要做这一步，处理一般输入串不需要这个步骤
		process_delim_matcher(&match_delim);

		if( split_string(&match_delim, &match_delim.ne_arg->tpool, cfg, model, &root) )
		{
			printf("error splitting rule: %s\n", match_delim.ori_str);
			goto loop;
		}

		if( process_rule_segorder(&match_delim, cfg, model, &root) < 0 )
		{
			printf("error processing segment order of rule: %s\n", match_delim.ori_str);
			goto loop;
		}

		rule_init(rule, match_delim.ori_str, cfg->cfg_rule_t_cfgs[i].normal_rule);

		if( process_wildcard(&match_delim, cfg, model, rule) < 0 )
		{
			printf("error processing wildcard of rule: %s\n", match_delim.ori_str);
			goto loop;
		}

		// 最后再分析一下normal_rule，建立通配之间的对应关系，也用AC做
		if( process_normal_rule(&match_delim, cfg, model, rule) < 0 )
		{
			printf("error processing normal rule of rule: %s\n", match_delim.ori_str);
			goto loop;
		}

		if( create_radix_mata(model, rule, ne_arg) < 0 )
		{
			printf("error create radix mata of rule: %s\n", match_delim.ori_str);
			goto loop;
		}

		rule->leading_grp = match_delim.leading_grp;

#ifdef _DEBUG_PRINT
		printf("rule: %s\n", match_delim.ori_str);
		print_delim_match_result(&match_delim, cfg);
		print_normal_rule(rule);
		printf("\n");
#endif

		++rule;

				
loop:	
		//ne_arg->tpool.cur_ptr = ne_arg->tpool.pool;
		dlist_init(&match_delim.delim_head);
		dlist_init(&match_delim.wc_head);
	}

	if( !model->rm_tree.root || rm_matafy(&model->rm_tree, sp_alloc, &ne_arg->spool, &ne_arg->tpool) < 0 )
	{
		printf("error create radix mata\n");
		return -1;
	}

	if( acsmCompile2(model->acmata.ac_handle, NULL, NULL) < 0 )
	{
		printf("error compile ac\n");
		return -1;
	}
	
	return 0;
}


normalize_engine_t*		build_normalize_engine(ne_cfg_t* cfg, M_sint32* memory_size, M_sint32* tmp_memory_size)
{
	ne_arg_t			ne_arg;
	pattern_t*			wc_pat;
	normalize_engine_t* model;

	*memory_size = cfg->cfg_common_t_cfgs->engine_size * 1024;

	if( ne_arg_init(&ne_arg, *memory_size, cfg->cfg_group_t_nr_sets) < 0 )
		return NULL;

	if( !(wc_pat = (pattern_t*)sp_alloc(sizeof(pattern_t)*NR_WILDCARD, &ne_arg.tpool)) )
		goto out;

	if( !(model = (normalize_engine_t*)sp_alloc(sizeof(normalize_engine_t), &ne_arg.spool)) )
		goto out;

	if( ne_model_init(model, &ne_arg, cfg) < 0 )
		goto out;

	if(!ne_arg.ac.ac_handle || !model->acmata.ac_handle)
		goto out;
	
	sort_cfg_group(cfg, model);
	if(model->default_grp == -1)
		goto out;

	if( parse_delims(model, cfg, &ne_arg/*, seg_pat*/) < 0 )
		goto out;

	//if( check_delims(model, cfg->cfg_group_t_nr_sets ) < 0 )
	//	goto out;

	if( add_wildcards(&ne_arg, wc_pat, cfg) < 0 )
		goto out;

	//build ac for delims, it would be used in parse rules: split_string
	if( acsmCompile2(ne_arg.ac.ac_handle, NULL, NULL) < 0 )
		goto out;

	//acsmPrintInfo2(ne_arg.ac.ac_handle);
	//acsmPrintDetailInfo2(ne_arg.ac.ac_handle);
	//acsmPrintSummaryInfo2();

	if( parse_rules(model, cfg, &ne_arg) < 0 )
		goto out;

	acsmFree2(ne_arg.ac.ac_handle);
	free(ne_arg.tpool.pool);
	*memory_size = sp_hwm(&ne_arg.spool) > 0 ? sp_hwm(&ne_arg.spool) : ne_arg.spool.cur_ptr - ne_arg.spool.pool;
	*tmp_memory_size = sp_hwm(&ne_arg.tpool) > 0 ? sp_hwm(&ne_arg.tpool) : ne_arg.tpool.cur_ptr - ne_arg.tpool.pool;
#ifdef _DEBUG_PRINT
	printf("%d/%d memory of spool, %ld/%d memory of tpool\n", *memory_size, sp_hwm(&ne_arg.spool),
		ne_arg.tpool.cur_ptr - ne_arg.tpool.pool, sp_hwm(&ne_arg.tpool));
#endif
	return model;

out:
	if(ne_arg.ac.ac_handle)
		acsmFree2(ne_arg.ac.ac_handle);
	if(model->acmata.ac_handle)
	{
		acsmFree2(model->acmata.ac_handle);
		model->acmata.ac_handle = NULL;
	}

	free(ne_arg.tpool.pool);
	destroy_normalize_engine(model);
	return NULL;
}

void	destroy_normalize_engine(normalize_engine_t* model)
{
	if(model->acmata.ac_handle)
		acsmFree2(model->acmata.ac_handle);
	free(model->memory);
}

#define NO_DUMMY		(-1)
#define INVALID_DUMMY	0
#define VALID_DUMMY		1
match_handle_t*	create_match_handle(normalize_engine_t* model, ne_cfg_t* cfg)
{
	M_sint8* memory;
	M_sint32 i;
	match_handle_t* match_handle;
	M_sint32 memory_size = cfg->cfg_common_t_cfgs->handle_size * 1024;

	if(sizeof(match_handle_t) > memory_size)
		return NULL;

	if(!(memory = (M_sint8*)malloc(memory_size)))
		return NULL;
	match_handle = (match_handle_t*)memory;

	match_handle->cfg = cfg;
	match_handle->model = model;
	sp_init(memory + sizeof(match_handle_t), memory_size - sizeof(match_handle_t), &match_handle->tpool);

	if( !(match_handle->handle = rm_init_handle_bypool(&match_handle->tpool, NULL)) )
		return NULL;

	for(i=0; i<2; ++i)
	{
		if( !(match_handle->left_dummy[i] = create_dummy_delim(-1, cfg->cfg_group_t_nr_sets, &match_handle->tpool)) )
			return NULL;
		match_handle->left_dummy[i]->color = i;
	}
	for(i=0; i<2; ++i)
	{
		if( !(match_handle->right_dummy[i] = create_dummy_delim(-1, cfg->cfg_group_t_nr_sets, &match_handle->tpool)) )
			return NULL;
		match_handle->right_dummy[i]->color = i;
	}

	match_handle->back_mem = match_handle->tpool.cur_ptr;

	return match_handle;
}

M_sint32			destroy_match_handle(match_handle_t* handle)
{
	M_sint32 ret = sp_hwm(&handle->tpool);
	free(handle);
	return ret;
}

void		set_match_handle(match_handle_t* match_handle, M_sint8* src_str, M_sint32 src_str_len)
{
	M_sint32 i;
	match_handle->tpool.cur_ptr = match_handle->back_mem;
	dlist_init(&match_handle->delim_head);
	slist_init(&match_handle->result_head);

	rm_handle_insert_string(match_handle->handle, src_str, src_str_len);
	//match_handle->dst_str = NULL;
	//match_handle->dst_str_len = 0;
	match_handle->status = -1;
	match_handle->nr_matched_rules = 0;
	match_handle->nr_delims = 0;

	for(i=0; i<2; ++i)
	{
		match_handle->left_dummy[i]->grp_id = -1;
		match_handle->right_dummy[i]->seg_pos = 0;
	}
}

static INLINE void print_ne_match_result(match_handle_t* match_handle)
{
	delim_pos_t* string_pos;

	M_dlist* pos_stub = match_handle->delim_head.next;
	while(pos_stub != &match_handle->delim_head)
	{
		string_pos = container_of(pos_stub, delim_pos_t, list_stub);
		printf("delim: %s, offset: %d, str_len: %d, address: 0x%p\n", 
			string_pos->delim_pat->str, string_pos->pos, string_pos->delim_pat->str_len, string_pos->delim_pat);
		pos_stub = pos_stub->next;
	}
}

/*
	id:		pattern_t 
	data:	match_ac_t

	callback for ac mata.
	return value > 0: break ac processing
				 = 0: continue ac processing

	由于这个版本的AC不能区分大小写，所以得自己判断
	由于AC_FULL模式会出来很多相同的匹配结果，所以得自己过滤
*/
static INLINE M_sint32	ac_matcher(pattern_t *id, void *tree, M_sint32 offset, match_handle_t *data, void *neg_list)
{
	delim_pos_t* string_pos;
	delim_pos_t* current;
	M_dlist*	list_stub;
	M_dlist*	list_head;

	//检查大小写是否匹配
	if( strncmp(id->str, data->handle->input_array + offset, id->str_len) )
		return 0;

	if( !(string_pos = (delim_pos_t*)sp_alloc(sizeof(delim_pos_t), &data->tpool)))
		return 1;
	
	assert(offset >= 0);
	memset(string_pos, 0, sizeof(delim_pos_t));
	//printf("matches %s, offset: %d, str len: %d, address: 0x%p\n", id->str, offset, id->str_len, id);

	string_pos->pos = offset;
	string_pos->delim_pat = id;
	current = string_pos;
	
	string_pos = current;
	list_head = &data->delim_head;
	dlist_append(list_head, &string_pos->list_stub);
	++data->nr_delims;

	list_stub = string_pos->list_stub.prev;
	while(list_stub != list_head)
	{
		string_pos = container_of(list_stub, delim_pos_t, list_stub);
		list_stub = list_stub->prev;

		//如果之前的匹配结果的offset比当前匹配结果的offset大，
		//表示有更长的匹配串，删除之前的匹配结果
		if(string_pos->pos >= offset)
		{
			dlist_remove(list_head, &string_pos->list_stub);
			--data->nr_delims;
		}
		else if(string_pos->pos + string_pos->delim_pat->str_len > offset)
		{
			//如果之前的匹配结果加上模式串长度大于当前的offset，表示有交叉发生，删除当前的匹配结果
			dlist_remove(list_head, &current->list_stub);
			--data->nr_delims;
			break;
		}
		else
			break;
	}

	return 0;
}

/*
	这两个有关match_delim的函数只给normalize_string用，其它地方不得使用
	为了偷懒，用了个贱招：把match_handle中的delim_head直接挪到match_delim中了，方便直接使用split_string
*/
static INLINE void	match_delim_init(mat_delim_t* match_delim, match_handle_t* match_handle)
{
	M_dlist *start, *end;

	dlist_init(&match_delim->delim_head);
	dlist_init(&match_delim->wc_head);
	dlist_init(&match_delim->rest_delim);
	dlist_init(&match_delim->rest_wc);
	match_delim->ne_arg = NULL;
	match_delim->escape_char = match_handle->cfg->cfg_common_t_cfgs->escape_char;

	match_delim->ori_str = match_handle->handle->input_array;
	match_delim->ori_str_len = match_handle->handle->input_len;
	match_delim->nr_wc = 0;
	match_delim->nr_delim = match_handle->nr_delims;
	match_delim->leading_grp = -1;

	start = match_handle->delim_head.next;
	end = match_handle->delim_head.prev;

	if(start != &match_handle->delim_head)
	{
		dlist_remove_list(&match_handle->delim_head, start, end);
		dlist_append_list(&match_delim->delim_head, start, end);
	}
}

static INLINE M_sint32 process_string_segorder(mat_delim_t* match_delim, ne_cfg_t* cfg, normalize_engine_t* model, M_stackpool* tpool, M_bst_stub** root)
{
	M_sint8*	buf;
	M_sint32	i = match_delim->leading_grp, j;
	M_sint32	start_pos, end_pos, dst_pos;
	M_sint32	pos_shift = 0;

	M_dlist		*delim_stub, *bak_delim_stub;
	delim_pos_t	*delim_pos, *head_delim_pos, *last_delim_pos, *insert_delim_pos;

	string_seg_t*	delim_pos_arr;	// 从match_delim中取出分隔符后筛选有效的放在这个数组中
	M_sint32		nr_pos;			// delim_pos_arr数组的有效长度
	M_dlist			delim_head;
	pattern_t		*head_pat;
	M_sint32		seg_pos;
	M_sint32		next_grp;
	M_sint32		is_start;		// 标记head_delim_pos的类型

	if(!model->disorder_seg || dlist_empty(&match_delim->delim_head))
		return 0;
	if( !(buf = (M_sint8*)sp_alloc(match_delim->ori_str_len, tpool)) )
		return -1;
	if( !(delim_pos_arr = (string_seg_t*)sp_alloc(sizeof(string_seg_t)*(match_delim->nr_delim+1), tpool)) )
		return -1;

	memcpy(buf, match_delim->ori_str, match_delim->ori_str_len);

	dst_pos = start_pos = 0;
	delim_stub = match_delim->delim_head.next;
	while(i<cfg->cfg_group_t_nr_sets)
	{
		// 只要对无序组排序即可
		if(!cfg->cfg_group_t_cfgs[i].seg_in_order)
		{
			dlist_init(&delim_head);
			bak_delim_stub = delim_stub->prev;

			while(delim_stub != &match_delim->delim_head)
			{
				delim_pos = container_of(delim_stub, delim_pos_t, list_stub);
				delim_stub = delim_stub->next;
				if(delim_pos->grp_id < i)
				{
					bak_delim_stub = delim_stub->prev;
					continue;
				}
				if(delim_pos->grp_id == i)
				{
					dlist_remove(&match_delim->delim_head, &delim_pos->list_stub);
					rbt_remove_node(root, &delim_pos->rbt_stub, get_rbcolor_delim_tree, set_rbcolor_delim_tree);
					dlist_append(&delim_head, &delim_pos->list_stub);
				}
				else
				{
					end_pos = delim_pos->pos;
					next_grp = delim_pos->grp_id;
					break;
				}
			}

			if(delim_stub == &match_delim->delim_head)
			{
				next_grp = cfg->cfg_group_t_nr_sets;
				end_pos = match_delim->ori_str_len;
			}

			if(dlist_empty(&delim_head))
			{
				++i;
				continue;
			}

			// 如果起头的是seg delim，创建一个dummy
			is_start = 1;
			head_delim_pos = container_of(delim_head.next, delim_pos_t, list_stub);
			if(head_delim_pos->seg_pos)
			{
				if( !(head_delim_pos = create_dummy_delim(i, cfg->cfg_group_t_nr_sets, tpool)) )
					return -1;
				dlist_insert(&delim_head, &head_delim_pos->list_stub);
				is_start = 0;
			}
			
			//备份起始分隔符信息。因为有了dummy_delim，heat_delim_pos一定在组首（有可能是dummy delim）
			nr_pos = 0;
			delim_stub = delim_head.next;
			head_pat = head_delim_pos->delim_pat;
			dst_pos = head_delim_pos->pos;
			
			// 将段加入到delim_pos_arr中
			while(delim_stub != &delim_head)
			{
				delim_pos = container_of(delim_stub, delim_pos_t, list_stub);
				delim_stub = delim_stub->next;
				
				// 先全部加到数组
				if(nr_pos > 0)
				{
					last_delim_pos = delim_pos_arr[nr_pos-1].delim_pos;
					delim_pos_arr[nr_pos-1].str_len = delim_pos->pos - last_delim_pos->pos 
						- last_delim_pos->delim_pat->str_len;
				}
				delim_pos_arr[nr_pos].delim_pos = delim_pos;
				delim_pos_arr[nr_pos].str = match_delim->ori_str + delim_pos->pos;
				++nr_pos;
			}

			last_delim_pos = delim_pos_arr[nr_pos-1].delim_pos;
			delim_pos_arr[nr_pos-1].str_len = end_pos - last_delim_pos->pos 
				- last_delim_pos->delim_pat->str_len;

			// 排序
			if(nr_pos > 1)
				qsort(delim_pos_arr, nr_pos, sizeof(string_seg_t), cmp_delim_pos);
			
			// 重构数据结构
			seg_pos = 0;
			if(is_start)
			{
				if( !(insert_delim_pos = (delim_pos_t*)sp_alloc(sizeof(delim_pos_t), tpool)) )
					return -1;
				insert_delim_pos->delim_pat = head_pat;
				if( !(bak_delim_stub = insert_delim(bak_delim_stub, insert_delim_pos, buf, &dst_pos, i, seg_pos, root)) )
					return -1;
			}
			memcpy(buf + dst_pos, delim_pos_arr[0].str + delim_pos_arr[0].delim_pos->delim_pat->str_len, delim_pos_arr[0].str_len);
			dst_pos += delim_pos_arr[0].str_len;

			++seg_pos;
			for(j=1; j<nr_pos; ++j)
			{
				// 借用head_pat
				if(delim_pos_arr[j].delim_pos->delim_pat == head_pat)
					delim_pos_arr[j].delim_pos->delim_pat = model->seg_pat[i];

				if( !(bak_delim_stub = insert_delim(bak_delim_stub, delim_pos_arr[j].delim_pos, buf, &dst_pos, i, seg_pos, root)) )
					return -1;
				memcpy(buf + dst_pos, delim_pos_arr[j].str + delim_pos_arr[j].delim_pos->delim_pat->str_len, delim_pos_arr[j].str_len);
				dst_pos += delim_pos_arr[j].str_len;
				++seg_pos;
			}

			delim_stub = bak_delim_stub->next;
			i = next_grp;
			start_pos = end_pos;
		}
		else
			++i;
		
	}
	sp_free(delim_pos_arr, tpool);
	
	match_delim->ori_str = buf;
	//match_delim->ori_str_len = dst_pos;
	match_delim->ori_str[dst_pos] = 0;

	return 0;
}

/*
	check_result的参数太多，专门用来传递参数的数据结构。。
*/
typedef struct st_check_arg
{
	M_sint32 wc_s, wc_e;	//rm_wc_info中wc_info的下标范围，在[s,e)这个区间的内容都属于同一组
	M_sint32 d_s, d_e;		//rm_wc_info中delim_info的下标范围，掐头去尾，同样前闭后开
	M_sint32 spos, epos;	//对应[s,e)的输入串中的区间（match_handle->handle->input_array）
							//区间不包括两端的delimiter
	delim_pos_t	*l, *r;		//[s,e)区间两侧距离区间最近的delim_pos。其中l->pos <= spos，
							//而r->pos 甚至可以小于epos，因为epos里包括r的长度。
							//分隔符都不属于掐头去尾后的区间
	rule_t*	rule;
	M_bst_stub* root;
	M_dlist*	delim_head;	//输入串的delim_pos链表
	M_dlist		group_delim_head;	//一个rm_wc_info对应的delim pos链表，两端是dummy节点或真实节点。保证两端节点一定存在
	M_sint32	leading_grp;
} check_arg_t;

static INLINE M_sint32 create_and_insert_wc_result_node(wc_info_t* wc_info, 
	M_sint32 start_pos, M_sint32 end_pos, match_handle_t* match_handle)
{
	M_rm_result_node* ori_wc_node;
	range_result_t*	rr = container_of(match_handle->result_head.next, range_result_t, list_stub);;
	assert(start_pos <= end_pos);
	if(wc_info->wc_seq >= 0)
	{
		ori_wc_node = rr->range_result[wc_info->wc_type] + wc_info->wc_seq;
		ori_wc_node->enter_pos = start_pos;
		ori_wc_node->leave_pos = end_pos;
		//ori_wc_node->wildcard = wc_info;//&rm_wc_info->wc_info[wc_index];
		//slist_insert(&wc_result->res_head, &ori_wc_node->res_stub);
	}
	return 0;

}

static INLINE M_sint32 check_seg_pos(delim_pos_t* delim_pos, wc_info_t* wc_info)
{
	if(wc_info->cmp_type == CT_EQUAL && delim_pos->seg_pos != wc_info->seg_pos)
		return 0;
	if(wc_info->cmp_type == CT_GREATER && delim_pos->seg_pos < wc_info->seg_pos)
		return 0;
	return 1;
}

static INLINE M_sint32 check_seg_part_of_rm_wc_from_right(rm_wc_info_t* rm_wc_info, check_arg_t* arg, 
	delim_pos_t* bound_delim_pos, match_handle_t* match_handle)
{
	M_sint32	i = arg->wc_e, j;
	M_sint32	wc_s = arg->wc_s;
	M_sint32	start_pos = bound_delim_pos->pos + bound_delim_pos->delim_pat->str_len;
	wc_info_t*	wc_info = &rm_wc_info->wc_info[i - 1];
	wc_info_t*	bak_wc_info = NULL;
	delim_info_t*	delim_info = &rm_wc_info->delim_info[arg->d_e-1];

	// 先获得wc_s
	if(arg->d_e > arg->d_s)
	{
		for(j=arg->wc_e-1; j>=arg->wc_s; --j)
		{
			if(wc_info->pos < delim_info->pos)
			{
				wc_s = j;
				break;
			}
			--wc_info;
		}
	}
	
	arg->epos -= arg->r->delim_pat->str_len;
	while(wc_s < arg->wc_e)
	{
		if(wc_info->wc_type == WT_SINGLECHAR)
		{
			if(create_and_insert_wc_result_node(wc_info, arg->epos - 1, arg->epos, match_handle) < 0)
				return -1;
			--arg->epos;
		}
		else if(wc_info->wc_type == WT_MULTICHAR)
		{
			bak_wc_info = wc_info;
			if( (i = wc_s) >= arg->wc_e)
				goto fail;

			break;
		}
		else
			goto fail;
		--arg->wc_e;
		--wc_info;
	}

	while(i < arg->wc_e)
	{
		if(wc_info->wc_type == WT_SINGLECHAR)
		{
			if(create_and_insert_wc_result_node(wc_info, start_pos, start_pos + 1, match_handle) < 0)
				return -1;
			++start_pos;
		}
		else if(wc_info->wc_type == WT_MULTICHAR)
		{
			if(wc_info != bak_wc_info)
				goto fail;
			else
			{
				if(create_and_insert_wc_result_node(wc_info, start_pos, arg->epos, match_handle) < 0)
					return -1;
			}
		}
		else
			goto fail;
		++i;
		++wc_info;
	}

	arg->wc_e = wc_s;
	arg->epos = bound_delim_pos->pos + bound_delim_pos->delim_pat->str_len;
	if(arg->d_s != arg->d_e && delim_info->pos > wc_info->pos)
		--arg->d_e;
	return 0;

fail:
	match_handle->status = -1;
	return 0;
}

/*
	进行段匹配时arg的参数中起始部分（_s系列）有效，结束部分（_e）系列无效。结束部分需要自己探测
	arg->l即为当前的delim_pos
	from_right则正好相反
*/
static INLINE M_sint32 check_seg_part_of_rm_wc_from_left(rm_wc_info_t* rm_wc_info, check_arg_t* arg, 
	delim_pos_t* bound_delim_pos, match_handle_t* match_handle)
{
	M_sint32	i = arg->wc_s;
	M_sint32	j;
	M_sint32	wc_e = arg->wc_e;
	M_sint32	end_pos = bound_delim_pos->pos;
	wc_info_t*	wc_info = &rm_wc_info->wc_info[i];
	wc_info_t*	bak_wc_info = NULL;
	delim_info_t*	delim_info = &rm_wc_info->delim_info[arg->d_s];

	// 先获得wc_e
	if(arg->d_e > arg->d_s)
	{
		for(j=arg->wc_s; j<arg->wc_e; ++j)
		{
			if(wc_info->pos > delim_info->pos)
			{
				wc_e = j;
				break;
			}
			++wc_info;
		}
	}
	
	wc_info = &rm_wc_info->wc_info[arg->wc_s];
	arg->spos += arg->l->delim_pat->str_len;
	while(arg->wc_s < wc_e)
	{
		if(wc_info->wc_type == WT_SINGLECHAR)
		{
			if(create_and_insert_wc_result_node(wc_info, arg->spos, arg->spos + 1, match_handle) < 0)
				return -1;
			++arg->spos;
		}
		else if(wc_info->wc_type == WT_MULTICHAR)
		{
			bak_wc_info = wc_info;
			if( (i = wc_e) <= arg->wc_s)
				goto fail;
			break;
		}
		else
			goto fail;
		++arg->wc_s;
		++wc_info;
	}

	while(arg->wc_s < i)
	{
		if(wc_info->wc_type == WT_SINGLECHAR)
		{
			if(create_and_insert_wc_result_node(wc_info, end_pos - 1, end_pos, match_handle) < 0)
				return -1;
			--end_pos;
		}
		else if(wc_info->wc_type == WT_MULTICHAR)
		{
			if(wc_info != bak_wc_info)
				goto fail;
			else
			{
				if(create_and_insert_wc_result_node(wc_info, arg->spos, end_pos, match_handle) < 0)
					return -1;
			}
		}
		else
			goto fail;
		--i;
		--wc_info;
	}

	arg->wc_s = wc_e;
	arg->spos = bound_delim_pos->pos;
	if(arg->d_s != arg->d_e && delim_info->pos < wc_info->pos)
		++arg->d_s;
	return 0;

fail:
	match_handle->status = -1;
	return 0;
}

/*
	将同组的内容分割成段
	以输入串为主线进行，先处理两种情况：
	1. deliminfo和wcinfo的组在输入串中不存在，检查是否仅为多段通配，否则匹配失败
	2. 双方对应，检查delim是否对应。这时以匹配规则为主线，检查是否有多段通配，如果有，两端逼近检查
*/
static INLINE M_sint32 check_grp_part_of_rm_wc(rm_wc_info_t* rm_wc_info, check_arg_t* arg,
	match_handle_t* match_handle)
{
	M_dlist*		list_stub = &arg->l->list_stub;
	delim_pos_t*	bound_delim_pos;
	delim_pos_t		*bak_arg_r, *prev_real_delim_pos;
	delim_info_t*	delim_info;
	wc_info_t*		wc_info;
	M_sint32		end_pos, i, j;
	M_sint32		reverse = 0;

	//start_pos = arg->spos;


	// 当多段通配匹配空串时，不检查两端的分隔符是否匹配

	// 若wc_info下标为i，其左边的分隔符下标为i+delim_shift-1，右边分隔符下标为i+delim_shift
	// 若deliminfo下标小于0，或大于等于nr_delims，表示不存在
	//delim_shift = rm_wc_info->delim_info[0].pos < rm_wc_info->wc_info[0].pos ? 1 : 0;

	if(arg->l == arg->r)
	{
		if(/*arg->wc_e - arg->wc_s != 1 || arg->d_e - arg->d_s > 2 || */arg->d_e == arg->d_s)
			goto fail;
		else
		{
			wc_info = &rm_wc_info->wc_info[arg->wc_s];
			j = arg->wc_s;
			for(i=arg->d_s; i<arg->d_e; ++i)
			{
				delim_info = &rm_wc_info->delim_info[i];
				if(arg->l->delim_pat == delim_info->delim_pat)
				{
					while(j < arg->wc_e && wc_info->pos < delim_info->pos)
					{
						if(wc_info->wc_type != WT_MULTISEG && j != arg->wc_s)
							goto fail;
						if( create_and_insert_wc_result_node(wc_info, arg->l->pos, arg->l->pos, match_handle) < 0)
							return -1;
						++wc_info;
						++j;
					}
					while(j < arg->wc_e)
					{
						if(wc_info->wc_type != WT_MULTISEG && j != arg->wc_e - 1)
							goto fail;
						if( create_and_insert_wc_result_node(wc_info, arg->l->pos + arg->l->delim_pat->str_len, 
							arg->l->pos + arg->l->delim_pat->str_len, match_handle) < 0)
							return -1;
						++wc_info;
						++j;
					}
				}
				++delim_info;
			}
			if(j == arg->wc_s)
				goto fail;
			return 0;
		}
	}
	else
	{
		wc_info = &rm_wc_info->wc_info[arg->wc_s];
		delim_info = &rm_wc_info->delim_info[arg->d_s];

		// 规则串中可能包含多个组，其组号都小于输入串中的组号
		while(arg->wc_s < arg->wc_e)
		{
			if(wc_info->grp < arg->l->grp_id)
			{
				if(wc_info->wc_type == WT_SINGLECHAR || wc_info->wc_type == WT_SINGLESEG)
					goto fail;
				if(wc_info->wc_type == WT_MULTICHAR && arg->wc_s)
					goto fail;
				if(create_and_insert_wc_result_node(wc_info, arg->spos, arg->spos, match_handle) < 0)
					return -1;
				++wc_info;
				++arg->wc_s;
			}
			else
				break;
		}
		if(arg->wc_s == arg->wc_e)
			goto fail;

		while(arg->d_s < arg->d_e)
		{
			if(delim_info->grp_id < arg->l->grp_id)
			{
				++delim_info;
				++arg->d_s;
			}
			else
				break;
		}

		while(list_stub != &arg->r->list_stub)
		{
			arg->l = container_of(list_stub, delim_pos_t, list_stub);
			list_stub = list_stub->next;
			wc_info = &rm_wc_info->wc_info[arg->wc_s];
			delim_info = &rm_wc_info->delim_info[arg->d_s];

			// 检查seg pos
			if(wc_info->cmp_type == CT_EQUAL && arg->l->seg_pos != wc_info->seg_pos)
				goto fail;
			if(wc_info->cmp_type == CT_GREATER && arg->l->seg_pos < wc_info->seg_pos)
				goto fail;

			// 检查delim是否匹配
			if(arg->l->delim_pat->str)
			{
				if(arg->d_s < arg->d_e)
				{
					if(delim_info->pos > wc_info->pos || arg->l->delim_pat != delim_info->delim_pat)
						goto fail;
				}
				else
				{
					if(wc_info->wc_type != WT_MULTISEG || arg->wc_s + 1 != arg->wc_e)
						goto fail;
					if(create_and_insert_wc_result_node(wc_info, arg->l->pos + arg->l->delim_pat->str_len, arg->r->pos, match_handle) < 0)
						return -1;
					continue;
				}
			}
			//if(arg->l->delim_pat->str && (arg->d_s == arg->d_e || delim_info->pos > wc_info->pos || arg->l->delim_pat != delim_info->delim_pat))
			//	goto fail;
			if(!arg->l->delim_pat->str && (arg->d_s < arg->d_e && delim_info->pos < wc_info->pos))
				goto fail;

			// 检查是否段通配
			bound_delim_pos = container_of(arg->l->list_stub.next, delim_pos_t, list_stub);
			switch(wc_info->wc_type)
			{
			case WT_SINGLESEG:
				if(!arg->l->delim_pat->str && arg->l->color == INVALID_DUMMY)
					goto fail;
				if(!bound_delim_pos->delim_pat->str && bound_delim_pos->color == INVALID_DUMMY)
					goto fail;
				if(create_and_insert_wc_result_node(wc_info, arg->spos, bound_delim_pos->pos, match_handle) < 0)
					return -1;

				++arg->wc_s;
				if(arg->d_s != arg->d_e && delim_info->pos < wc_info->pos)
					++arg->d_s;
				arg->spos = bound_delim_pos->pos;
				break;

			case WT_MULTISEG:
				// 准备捅菊花。。
				if(!arg->l->delim_pat->str && arg->l->color == INVALID_DUMMY)
					goto fail;
				list_stub = &arg->r->list_stub;
				reverse = 1;
				break;

			default:
				if( check_seg_part_of_rm_wc_from_left(rm_wc_info, arg, bound_delim_pos, match_handle) < 0 )
					return -1;
				if( match_handle->status < 0 )
					goto fail;
			}
		}

		if(reverse)
		{
			i = arg->wc_e;
			j = arg->d_e;
			end_pos = arg->epos;
			bak_arg_r = arg->r;

			wc_info = &rm_wc_info->wc_info[arg->wc_e - 1];
			delim_info = &rm_wc_info->delim_info[arg->d_e - 1];

			// 规则串中可能包含多个组，其组号都大于输入串中的组号
			while(arg->wc_s < arg->wc_e)
			{
				if(wc_info->grp > arg->l->grp_id)
				{
					if(wc_info->wc_type == WT_SINGLECHAR || wc_info->wc_type == WT_SINGLESEG)
						goto fail;
					if(wc_info->wc_type == WT_MULTICHAR && arg->wc_e < rm_wc_info->nr_wcs)
						goto fail;
					if(create_and_insert_wc_result_node(wc_info, arg->epos, arg->epos, match_handle) < 0)
						return -1;

					--wc_info;
					--arg->wc_e;
				}
				else
					break;
			}
			if(arg->wc_s == arg->wc_e)
				goto fail;

			while(arg->d_s < arg->d_e)
			{
				if(delim_info->grp_id > arg->l->grp_id)
				{
					--delim_info;
					--arg->d_e;
				}
				else
					break;
			}
			while(list_stub != &arg->l->list_stub)
			{
				arg->r = container_of(list_stub, delim_pos_t, list_stub);
				list_stub = list_stub->prev;
				prev_real_delim_pos = container_of(list_stub, delim_pos_t, list_stub);

				wc_info = &rm_wc_info->wc_info[arg->wc_e - 1];
				delim_info = &rm_wc_info->delim_info[arg->d_e - 1];

				// 检查seg pos
				if(wc_info->cmp_type == CT_EQUAL && prev_real_delim_pos->seg_pos != wc_info->seg_pos)
					goto fail;
				if(wc_info->cmp_type == CT_GREATER && prev_real_delim_pos->seg_pos < wc_info->seg_pos)
					goto fail;

				// 检查delim是否匹配
				if(arg->r->delim_pat->str && arg->r->grp_id == arg->l->grp_id)
				{
					if(arg->d_s < arg->d_e)
					{
						if(delim_info->pos < wc_info->pos || arg->r->delim_pat != delim_info->delim_pat)
							goto fail;
					}
					else
					{
						if(wc_info->wc_type != WT_MULTISEG || arg->wc_s + 1 != arg->wc_e)
							goto fail;
						if(create_and_insert_wc_result_node(wc_info, arg->l->pos + arg->l->delim_pat->str_len, arg->r->pos, match_handle) < 0)
							return -1;
						continue;
					}
				}
				//if(real_delim_pos->delim_pat->str && (arg->d_s == arg->d_e || delim_info->pos < wc_info->pos || real_delim_pos->delim_pat != delim_info->delim_pat))
				//	goto fail;
				if(!arg->r->delim_pat->str && (arg->d_s < arg->d_e && delim_info->pos > wc_info->pos))
					goto fail;

				// 检查是否段通配
				bound_delim_pos = container_of(arg->r->list_stub.prev, delim_pos_t, list_stub);
				switch(wc_info->wc_type)
				{
				case WT_SINGLESEG:
					if(!arg->r->delim_pat->str && arg->r->color == INVALID_DUMMY)
						goto fail;
					if(!bound_delim_pos->delim_pat->str && bound_delim_pos->color == INVALID_DUMMY)
						goto fail;
					arg->epos = bound_delim_pos->pos + bound_delim_pos->delim_pat->str_len;
					if(create_and_insert_wc_result_node(wc_info, arg->epos, arg->r->pos, match_handle) < 0)
						return -1;

					--arg->wc_e;
					if(arg->d_s != arg->d_e && delim_info->pos > wc_info->pos)
						--arg->d_e;
					break;

				case WT_MULTISEG:
					if(create_and_insert_wc_result_node(wc_info, 
						arg->l->pos + arg->l->delim_pat->str_len, arg->r->pos, match_handle) < 0)
						return -1;

					arg->wc_e = i;
					arg->d_e = j;
					arg->epos = end_pos;
					arg->r = bak_arg_r;
					goto out;

				default:
					if( check_seg_part_of_rm_wc_from_right(rm_wc_info, arg, bound_delim_pos, match_handle) < 0 )
						return -1;
					if( match_handle->status < 0 )
						goto fail;
					

				}
			}
			arg->wc_e = i;
			arg->d_e = j;
			arg->epos = end_pos;
			arg->r = bak_arg_r;
		}
		else	// 处理最后没有匹配完的通配
		{
			wc_info = &rm_wc_info->wc_info[arg->wc_s];
			for(i=arg->wc_s; i<arg->wc_e; ++i)
			{
				if(wc_info->wc_type != WT_MULTISEG)
					goto fail;
				else
				{
					if(create_and_insert_wc_result_node(wc_info, arg->r->pos, arg->r->pos, match_handle) < 0)
						return -1;
					break;
				}

			}
		}
	}
out:
	return 0;

fail:
	if(!match_handle->status)
		match_handle->status = -1;
	return 0;
}

/*
	本函数处理一个rm通配，主要做归整，以及按组分段工作，方面后面处理
	下面还有两个函数：
	check_grp_part_of_rm_wc: 处理rm通配中同组内容
	check_seg_part_of_rm_wc：处理rm通配中同段内容
	两个函数方法类似
*/

static INLINE M_sint32 check_rm_wc(check_arg_t* arg, M_rm_result_node* match_wc, rm_wc_info_t* rm_wc_info, 
	match_handle_t* match_handle)
{
	M_sint32	grp;
	M_bst_stub*	rbt_stub;
	M_dlist		*list_stub, *bak_list_stub = NULL;
	M_dlist		*left_stub, *right_stub;
	M_sint32	left_type, right_type;

	delim_pos_t	*delim_pos;

	dlist_init(&arg->group_delim_head);

	// 处理输入串匹配为空的情况
	if(match_wc->leave_pos == match_wc->enter_pos)
	{
		if(rm_wc_info->nr_wcs > 1 || rm_wc_info->nr_delims > 2)
			goto fail;
		else
		{
			if( create_and_insert_wc_result_node(&rm_wc_info->wc_info[0], match_wc->enter_pos, 
				match_wc->leave_pos, match_handle) < 0)
				return -1;

			return 0;
		}
	}

	/*
		找到匹配区间两端的分隔符，用来确定组、段位置。这里都用lt
	*/
	left_stub = right_stub = NULL;
	left_type = right_type = NO_DUMMY;
	if( (rbt_stub = bst_search_lt(arg->root, &match_wc->enter_pos, cmp_key_delim_tree, get_key_delim_tree)) )
		left_stub = &container_of(rbt_stub, delim_pos_t, rbt_stub)->list_stub;
	if( (rbt_stub = bst_search_lt(arg->root, &match_wc->leave_pos, cmp_key_delim_tree, get_key_delim_tree)) )
		right_stub = &container_of(rbt_stub, delim_pos_t, rbt_stub)->list_stub;

	/*
		如果左边找到了，有两种情况：
			1. delim在范围内，它必然是边界，此时不需要加入dummy（dummy就是为了刻画边界），且delim就是内部起始delim
			2. delim不在范围内，此时需要加入dummy，还要找到内部起始delim
		如果左边没找到，此时enter pos必然在第一组第一段，需要构造dummy，也要找到内部起始delim
	*/
	if(left_stub)
	{
		delim_pos = container_of(left_stub, delim_pos_t, list_stub);
		//if(delim_pos->pos + delim_pos->delim_pat->str_len >= match_wc->leave_pos)
		//	goto fail;
		if(delim_pos->pos < match_wc->enter_pos)
		{
			if(delim_pos->pos + delim_pos->delim_pat->str_len == match_wc->enter_pos)
				left_type = VALID_DUMMY;
			else
				left_type = INVALID_DUMMY;
			match_handle->left_dummy[left_type]->pos = match_wc->enter_pos;
			match_handle->left_dummy[left_type]->grp_id = delim_pos->grp_id;
			match_handle->left_dummy[left_type]->seg_pos = delim_pos->seg_pos;
			if( (left_stub = left_stub->next) == arg->delim_head)
				left_stub = NULL;
			else
			{
				delim_pos = container_of(left_stub, delim_pos_t, list_stub);
				if(delim_pos->pos >= match_wc->leave_pos)
					left_stub = NULL;
			}
		}
		//else delim_pos->pos == match_wc->enter_pos, NO_DUMMY
	}
	else
	{
		if( (left_stub = arg->delim_head->next) == arg->delim_head )
			left_stub = NULL;
		else
		{
			delim_pos = container_of(left_stub, delim_pos_t, list_stub);
			if(delim_pos->pos >= match_wc->leave_pos)
				left_stub = NULL;
		}
		left_type = match_wc->enter_pos ? INVALID_DUMMY : VALID_DUMMY;
		match_handle->left_dummy[left_type]->pos = match_wc->enter_pos;
		match_handle->left_dummy[left_type]->grp_id = arg->leading_grp;
		match_handle->left_dummy[left_type]->seg_pos = 0;
	}
	
	/*
		如果右边找到了，有三种情况：
			1. 它是边界（必然在内部），此时什么也不用做
			2. 它不是边界，在内部，要构造dummy，这个找到的就是内部终止delim
			3. 它不在内部，要构造dummy，内部无delim。
		如果右边找不到，要构造dummy，内部无delim。此时通配对应范围必然是第一组第一段，且不与第一个分隔符相邻

		如果left_stub == NULL，那么right_stub必然也是NULL。因为到了这里left_stub指向匹配范围内部的分隔符
	*/
	if(right_stub)
	{
		delim_pos = container_of(right_stub, delim_pos_t, list_stub);
		if(delim_pos->pos == match_wc->leave_pos)
		{
			right_type = VALID_DUMMY;
			if( (right_stub = right_stub->prev) == arg->delim_head)
				right_stub = NULL;
			else
			{
				delim_pos = container_of(right_stub, delim_pos_t, list_stub);
				if(delim_pos->pos < match_wc->enter_pos)
					right_stub = NULL;
			}
		}
		else
		{
			if(delim_pos->pos < match_wc->enter_pos)
			{
				right_stub = NULL;
				right_type = INVALID_DUMMY;
			}
			else if(delim_pos->pos + delim_pos->delim_pat->str_len < match_wc->leave_pos)
				right_type = INVALID_DUMMY;
			//else		//右边已经是边界，且在范围内，不需要加入dummy
		}
		
		if(right_type != NO_DUMMY)
		{
			match_handle->right_dummy[right_type]->pos = match_wc->leave_pos;
			match_handle->right_dummy[right_type]->grp_id = delim_pos->grp_id;
			match_handle->right_dummy[right_type]->seg_pos = delim_pos->seg_pos;
		}
		
	}
	else	// 必然是第一组第一段。因为空匹配之前已经处理了，这里不可能是空匹配
	{
		right_type = INVALID_DUMMY;
		match_handle->right_dummy[right_type]->pos = match_wc->leave_pos;
		match_handle->right_dummy[right_type]->grp_id = arg->leading_grp;
		match_handle->right_dummy[right_type]->seg_pos = 0;
	}

	// 取出链表，构建新链表
	if(left_stub)
	{
		assert(right_stub);
		assert(container_of(left_stub, delim_pos_t, list_stub)->pos >= match_wc->enter_pos);
		assert(container_of(left_stub, delim_pos_t, list_stub)->pos < match_wc->leave_pos);
		assert(container_of(right_stub, delim_pos_t, list_stub)->pos >= match_wc->enter_pos);
		assert(container_of(right_stub, delim_pos_t, list_stub)->pos < match_wc->leave_pos);

		bak_list_stub = left_stub->prev;
		dlist_remove_list(arg->delim_head, left_stub, right_stub);
		dlist_append_list(&arg->group_delim_head, left_stub, right_stub);

		//while(left_stub != right_stub)
		//{
		//	rbt_remove_node(&arg->root, container_of(left_stub, delim_pos_t, list_stub), get_rbcolor_delim_tree, set_rbcolor_delim_tree);
		//	left_stub = left_stub->next;
		//}
		//rbt_remove_node(&arg->root, container_of(left_stub, delim_pos_t, list_stub), get_rbcolor_delim_tree, set_rbcolor_delim_tree);

		//printf("%s\n", match_handle->handle->input_array);
		//delim_stub = bst_get_first(arg->root);
		//while(delim_stub)
		//{
		//	delim_pos = container_of(delim_stub, delim_pos_t, rbt_stub);
		//	printf("%d(%p) ", delim_pos->pos, delim_pos);
		//	delim_stub = bst_successor(delim_stub);
		//}
		//printf("\n");

		//delim_stub = arg->delim_head->next;
		//while(delim_stub != arg->delim_head)
		//{
		//	delim_pos = container_of(delim_stub, delim_pos_t, list_stub);
		//	printf("%d(%p) ", delim_pos->pos, delim_pos);
		//	delim_stub = ((M_dlist*)delim_stub)->next;
		//}
		//printf("\n");
	}
	else
		assert(!right_stub);

	if(left_type != NO_DUMMY)
		dlist_insert(&arg->group_delim_head, &match_handle->left_dummy[left_type]->list_stub);
	if(right_type != NO_DUMMY)
		dlist_append(&arg->group_delim_head, &match_handle->right_dummy[right_type]->list_stub);

	// 划分组
	list_stub = arg->group_delim_head.next;
	arg->l = container_of(list_stub, delim_pos_t, list_stub);
	grp = arg->l->grp_id;
	arg->wc_s = 0;
	arg->d_s = 0;
	arg->spos = match_wc->enter_pos;
	while(list_stub != &arg->group_delim_head)
	{
		while(list_stub != &arg->group_delim_head)
		{
			arg->r = container_of(list_stub, delim_pos_t, list_stub);
			arg->epos = arg->r->pos;
			if(arg->r->grp_id > grp)
				break;
			list_stub = list_stub->next;
		}

		for(arg->wc_e = arg->wc_s; arg->wc_e < rm_wc_info->nr_wcs; ++arg->wc_e)
		{
			if(rm_wc_info->wc_info[arg->wc_e].grp > grp)
				break;
		}

		for(arg->d_e = arg->d_s; arg->d_e < rm_wc_info->nr_delims; ++arg->d_e)
		{
			if(rm_wc_info->delim_info[arg->d_e].grp_id > grp)
				break;
		}

		if( check_grp_part_of_rm_wc(rm_wc_info, arg, match_handle) < 0 )
			return -1;
		if( match_handle->status < 0 )
			goto fail;

		arg->wc_s = arg->wc_e;
		arg->d_s = arg->d_e;
		arg->spos = arg->epos;
		arg->l = arg->r;
		//arg->r = arg->l;
		grp = arg->l->grp_id;
	}

	if(bak_list_stub)
		dlist_insert_list(bak_list_stub, left_stub, right_stub);
	return 0;

fail:
	if(bak_list_stub)
		dlist_insert_list(bak_list_stub, left_stub, right_stub);
	match_handle->status = -1;
	return 0;
}

/*
	检查匹配的rule是否满足规则要求
	返回0表示成功，-1表示失败
*/
static INLINE M_sint32 check_result(check_arg_t* arg, match_handle_t* match_handle, M_sint32 mode)
{
	M_dlist*	rule_stub = match_handle->rm_result.next;
	M_slist*	wc_stub;
	M_rm_result*		match_rule;	
	M_rm_result_node*	match_wc;
	rm_wc_info_t*		rm_wc_info;
	range_result_t*		rr;
	
	M_sint32 min_wcs = -1;
	M_sint32 max_match_rule_len = -1;
	M_sint32 i;

	while(rule_stub != &match_handle->rm_result)
	{
		match_handle->status = 0;
		match_rule = container_of(rule_stub, M_rm_result, match_stub);
		rule_stub = rule_stub->next;
		arg->rule = (rule_t*)match_rule->rule;
		if(match_rule->nr_wcs != arg->rule->nr_rm_wc)
		{
			dlist_remove(&match_handle->rm_result, &match_rule->match_stub);
			continue;
		}

		if( !(rr = (range_result_t*)sp_alloc(sizeof(range_result_t), &match_handle->tpool)) )
			return -1;

		if( !(rr->range_result[0] = (M_rm_result_node*)sp_alloc(sizeof(M_rm_result_node)*arg->rule->nr_ori_wc*4, &match_handle->tpool)) )
			return -1;
		for(i=1; i<4; ++i)
			rr->range_result[i] = rr->range_result[i-1] + arg->rule->nr_ori_wc;
		rr->matched_rule = arg->rule;

		slist_insert(&match_handle->result_head, &rr->list_stub);
		
		//痛苦的检查，很多种情况。。。
		i = 1;
		wc_stub = match_rule->res_head.next;
		rm_wc_info = arg->rule->rm_wc_arr;
		while(wc_stub != &match_rule->res_head)
		{
			match_wc = container_of(wc_stub, M_rm_result_node, res_stub);
			wc_stub = wc_stub->next;
			
			if( check_rm_wc(arg, match_wc, rm_wc_info, match_handle) < 0 )
				return -1;
			if( match_handle->status < 0 )
			{
				slist_remove(&match_handle->result_head);
				sp_free(rr->range_result[0], &match_handle->tpool);
				i = 0;
				break;
			}
			++rm_wc_info;
		}
		if(i)
		{
			if(min_wcs == -1)
			{
				min_wcs = arg->rule->nr_ori_wc;
				max_match_rule_len = arg->rule->match_rule_len;
				++match_handle->nr_matched_rules;
			}
			else if(mode == MM_BESTMATCH)
			{
				if(arg->rule->nr_ori_wc < min_wcs || (arg->rule->nr_ori_wc == min_wcs && 
					arg->rule->match_rule_len > max_match_rule_len))
				{
					while(!slist_empty(&match_handle->result_head))
						slist_remove(&match_handle->result_head);
					slist_insert(&match_handle->result_head, &rr->list_stub);
					min_wcs = arg->rule->nr_ori_wc;
					max_match_rule_len = arg->rule->match_rule_len;
					match_handle->nr_matched_rules = 1;
				}
				else if(arg->rule->nr_ori_wc == min_wcs && arg->rule->match_rule_len == max_match_rule_len)
				{
					++match_handle->nr_matched_rules;
				}
				else
				{
					slist_remove(&match_handle->result_head);
					sp_free(rr->range_result[0], &match_handle->tpool);
				}
			}
			else
				++match_handle->nr_matched_rules;
		}
	}

	return 0;
}

M_sint8*	get_normalize_string(match_handle_t* match_handle, M_sint32 rule_id, rule_t** rule, M_sint32* str_len)
{
	M_slist*	nm_stub;
	M_slist*	rr_stub = match_handle->result_head.next;
	normal_info_t* normal_info;
	M_sint32	pos = 0;
	M_sint8		*str, *tmp_str;
	M_sint32	wc_range_len;
	M_rm_result_node* rm_result_node;
	range_result_t*	rr;
	M_sint32	i;

	if(rule_id >= match_handle->nr_matched_rules)
		return NULL;

	for(i=0; i<rule_id; ++i)
		rr_stub = rr_stub->next;

	rr = container_of(rr_stub, range_result_t, list_stub);
	*rule = rr->matched_rule;

	if( !(str = sp_alloc(strlen(rr->matched_rule->normal_rule) + strlen(rr->matched_rule->match_rule), &match_handle->tpool)) )
		return NULL;

	nm_stub = rr->matched_rule->nm_head.next;
	while(nm_stub != &rr->matched_rule->nm_head)
	{
		normal_info = container_of(nm_stub, normal_info_t, nm_stub);
		nm_stub = nm_stub->next;

		M_snprintf(str + pos, normal_info->str_len + 1, "%s", normal_info->str);
		pos += normal_info->str_len;
		if(normal_info->next_wc)
		{
			//rr = container_of(match_handle->result_head.next, range_result_t, list_stub);
			rm_result_node = rr->range_result[normal_info->next_wc->wc_type] + normal_info->next_wc->wc_seq;
			tmp_str = match_handle->handle->input_array + rm_result_node->enter_pos;
			wc_range_len = rm_result_node->leave_pos - rm_result_node->enter_pos;
			M_snprintf(str + pos, wc_range_len + 1, "%s", tmp_str);
			pos += wc_range_len;
		}
	}

	*str_len = pos;
	return str;
}

static void* get_rule_string(rule_t* rule)
{
	return rule->match_rule;
}

/*
	先过radix mata，获得候选rule，再split string，判断是否符合rule要求
*/
M_sint32	normalize_string(match_handle_t* match_handle, M_sint32 mode)
{
	mat_delim_t	match_delim;
	M_bst_stub* root;
	check_arg_t arg;
	//M_bst_stub* delim_stub;
	//delim_pos_t* delim_pos;

	match_handle->status = -1;

	search_ac(match_handle->model->acmata.ac_handle, match_handle->handle->input_array, 
		match_handle->handle->input_len, match_handle, ac_matcher);

	match_delim_init(&match_delim, match_handle);

	if( split_string(&match_delim, &match_handle->tpool, match_handle->cfg, match_handle->model, &root) < 0 )
		return -1;

	if( process_string_segorder(&match_delim, match_handle->cfg, match_handle->model, &match_handle->tpool, &root) < 0 )
		return -1;

	match_handle->handle->input_array = match_delim.ori_str;
	if( rm_match(&match_handle->model->rm_tree, match_handle->handle) < 0 )
		return -1;

	if(dlist_empty(&match_handle->handle->match_head))
	{
#ifdef _DEBUG_PRINT
		printf("%s match rule fail!\n", match_handle->handle->input_array);
#endif
		return 0;
	}

	if( rm_parse_result(&match_handle->model->rm_tree, match_handle->handle, &match_handle->rm_result) < 0 )
		return -1;

#ifdef _DEBUG_PRINT
	printf("ori str:	%s\n", match_handle->handle->input_array);
	rm_print_result(&match_handle->model->rm_tree, match_handle->handle, &match_handle->rm_result, get_rule_string, stdout);
	//print_delim_match_result(&match_delim, match_handle->cfg);
#endif

	arg.root = root;
	arg.delim_head = &match_delim.delim_head;
	arg.leading_grp = match_delim.leading_grp;
	if( check_result(&arg, match_handle, mode) < 0 )
		return -1;

	return match_handle->nr_matched_rules;
}
