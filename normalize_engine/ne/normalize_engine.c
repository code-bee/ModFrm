
#include "normalize_engine.h"

config_set_t g_common_config[] = 
{
	{"group_numbers",	int_reader,			offset_of(cfg_common_t, nr_groups)},
	{"pattern_id_len",	int_reader,			offset_of(cfg_common_t, pattern_id_len)},
	{"group_order",		string_reader,		offset_of(cfg_common_t, group_order)},
	{"default_group",	string_reader,		offset_of(cfg_common_t, default_group)}
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

//static ne_cfg_t	s_cfg;

M_sint32 read_ne_config(char* filename, ne_cfg_t* cfgs)
{
	return read_config(filename, g_config, sizeof(g_config)/sizeof(config_t), cfgs);
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
		printf("pattern_id_len = %d\n\n", common->pattern_id_len);
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

//int	product_url_load_rule(const char* file_name, product_url_handle* handle)
//{}
//int product_url_normalize(product_url_handle* handle, char* src, char* dst)
//{}
//int	product_url_destroy_rule(product_url_handle* handle)
//{}

/*
	a helper structure for recording position of delimiters,
	is used to parse input string
*/
typedef struct st_delim_pos
{
	M_slist			list_stub;		// list stub used in delim_param_t
	delim_cons_t*	delim;			// what the delim is
	M_sint16		pos;			// where it occurs in input string
	M_sint16		type;			// final type of delim after input string is parsed(parse_string is called)
} delim_pos_t;

typedef struct st_delim_param
{
	M_slist			list_head;
} delim_param_t;

typedef struct st_ne_arg
{
	M_stackpool	spool;		//alloc memory for persistent usage, such as radix tree
	M_stackpool tpool;		//alloc memory for temporary usage
	M_rt_pool	pat_pool;
	M_rt_arg	extra_arg;
	acmata_t	ac;			//temporary ac mata for delimiters, to split rules
} ne_arg_t;

// keep delimiter matching result
typedef struct st_mat_delim
{
	M_slist		delim_head;
	ne_arg_t*	ne_arg;
} mat_delim_t;

// element of delim_head
typedef struct st_mat_pos
{
	M_slist		pos_stub;
	M_sint32	offset;
	pattern_t*	pat;
} mat_pos_t;

/*
	grp_info: fixed length array, its length is number of all groups
	if a group is not occured, grp member of corresponding grp_info is NULL
	
	该数据结构用于得到解析输入串的组、段位置，中间数据
	数据结构中的pos都是去掉了分隔符的位置。
	段分隔符的数组长度等于2*seg_num

	举例说明：

	a//b如果以//为组分隔符，那么
	组a的start_pos = 0, end_pos = 1, 组b的start_pos = 3, end_pos = 4

	a//b如果以//为段分隔符，那么seg_num=2，seg_pos数组的内容为：
	0,1,3,4
*/
typedef struct st_candidate_group
{
	M_slist		grp_stub;
	group_t*	grp;
} candidate_group_t;

typedef struct st_group_pos_info
{
	group_t*	grp;
	M_sint32	start_pos;
	M_sint32	end_pos;
	M_sint32	seg_num;
	M_sint32*	seg_pos;
	M_slist		cgrp_head;		//保存candidate group链表头
	M_sint32	nr_candidates;	//candidate group的数目
} group_pos_info_t;

typedef struct st_str_pos_info
{
	group_pos_info_t*	grp_info;
} str_pos_info_t;

// pattern type
#define PT_STRING	1
#define	PT_DELIM	2

// cons type
#define ET_STRING			1
#define	ET_DELIM			2
#define	ET_WILDCARD			3
#define ET_SPECIAL_DELIM	4

// string type, or seg type
#define ST_COMPLETE	0x01
#define ST_GREATER	0x02

// delim type
// DT_START, DT_END, DT_SEG are used for normal delim, can be combined together
// DT_HAT, DT_DOLLAR are used for special delim ^/$, can not be combined together
#define DT_START	0x01
#define DT_END		0x02
#define DT_SEG		0x04
#define DT_HAT		0x08
#define	DT_DOLLAR	0x10

// wildcard type
#define	WT_SINGLESEG	1
#define	WT_MULTISEG		2
#define	WT_SINGLECHAR	3
#define	WT_MULTICHAR	4

#define SDT_HAT		1
#define SDT_DOLLAR	2

/*
	p: via pat_stub
	e: via ele_stub
*/
#define	get_delim_p(list_stub)	container_of(list_stub, delim_cons_t, pat_stub)
//#define get_delim_e(list_stub)	container_of(list_stub, delim_cons_t, ele_stub)

#define get_str_p(list_stub)	container_of(list_stub, str_cons_t, pat_stub)
#define get_str_e(list_stub)	container_of(list_stub, str_cons_t, ele_stub)

#define get_wc_e(list_stub)		container_of(list_stub, wc_cons_t, ele_stub)

static INLINE M_sint8*	get_delim(M_sint8* str, M_sint8 c, M_sint8* buf, M_sint32* special_flag, M_sint16* str_len);

/*
	一组用于解析输入串的函数，目的在于将AC状态机匹配得到的各种delim组织起来，
	并利用其进行输入串分组、分段。当输入串解析完成后，基本上这个param就可以释放了。

	在这里用poolinf管理delim_pos_t
*/
static INLINE M_sint32	delim_param_init(delim_param_t* param);
static INLINE M_sint32	delim_param_insert(delim_param_t* param, delim_cons_t* delim, ne_arg_t* ne_arg);
static INLINE M_sint32	delim_param_destroy(delim_param_t* param, ne_arg_t* ne_arg);


/*
	解析得到的结果仍然放在param中，type域将会有有效数据。
	解析中认为非法的delim将会被删除
*/
static INLINE M_sint32	parse_string(M_sint8* str, M_sint32 str_len, delim_param_t* param);

static INLINE M_sint32	t_model_init(thread_ne_t* t_model);

static INLINE M_sint32	str_pat_insert(M_sint8* str, M_sint32 str_len, thread_ne_t* t_model);
static INLINE M_sint32	delim_pat_insert(M_sint8* delim, M_sint32 delim_len, thread_ne_t* model);
static INLINE M_sint32	rule_insert();
static INLINE M_sint32	build_acmata(normalize_engine_t* model);

/*
	把各组按照定义的group order进行排序
*/
static INLINE void sort_cfg_group(ne_cfg_t* cfg)
{
	cfg_group_t bak_grp;
	M_sint8	cur_grp[FLAG_LEN];
	M_sint8* grp_name;
	M_sint32 i,j;
	
	j = 0;
	grp_name = cfg->cfg_common_t_cfgs->group_order;
	while( (grp_name = get_delim(grp_name, ',', cur_grp, NULL, NULL)) )
	{
		for(i=j; i<cfg->cfg_common_t_cfgs->nr_groups; i++)
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
}

static INLINE void ne_model_init(normalize_engine_t* model, ne_arg_t* ne_arg)
{
	model->acmata.ac_handle = acsmNew2(NULL, NULL, NULL);
	model->delim_set = NULL;
	model->nr_rules = 0;
	model->pat_tree = NULL;
	model->rules = 0;
	model->memory = ne_arg->spool.pool;
}
static INLINE void ne_arg_init(ne_arg_t* ne_arg, M_sint32 pool_size)
{
	void* mem = malloc(pool_size);
	void* mem2 = malloc(pool_size);

	ne_arg->ac.ac_handle = acsmNew2(NULL, NULL, NULL);

	sp_init(mem, pool_size, &ne_arg->spool);
	sp_init(mem2, pool_size, &ne_arg->tpool);

	rt_init_pool(&ne_arg->pat_pool, (M_sint32)offset_of(pattern_t, rt_stub), 10);
	rt_pool_attach(&ne_arg->pat_pool, &ne_arg->spool, sp_alloc, sp_free);

	ne_arg->extra_arg.dummy_node = ne_arg->extra_arg.extra_node = NULL;
	rt_process_arg(&ne_arg->pat_pool, &ne_arg->extra_arg);
}

//static INLINE void ne_arg_free(ne_arg_t* ne_arg)
//{
//	rt_free_arg(&ne_arg->pat_pool, &ne_arg->extra_arg);
//	rt_destroy_pool(&ne_arg->pat_pool);
//	sp_destroy(&ne_arg->spool);
//	free(ne_arg->spool.pool);
//}

static INLINE void	pat_init(pattern_t* pat)
{
	memset(pat, 0, sizeof(pattern_t));
	slist_init(&pat->cons_head);
}

static INLINE void delim_node_init(delim_cons_t* delim_node, pattern_t* pat, cfg_group_t* grp)
{
	memset(delim_node, 0, sizeof(delim_cons_t));
	delim_node->grp = grp;
	delim_node->pat = pat;
	delim_node->ele_type = ET_DELIM;
	slist_insert(&pat->cons_head, &delim_node->pat_stub);
}

/*
	从配置文件的delim字段中解析出各个分隔符，需要处理字符转义问题
	取出的字符串从buf中返回
	special_flag: SDT_HAT -- 得到特殊字符^
				  SDT_DOLLAR -- 得到特殊字符$
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
			case '$':
				*buf++ = '$';
				break;
			case '^':
				*buf++ = '^';
				break;
			default:
				assert(0);
			}
		}
		else if(*str == '^')
		{
			assert(special_flag);
			*special_flag = SDT_HAT;
			if(*(str+1) != c)
				printf("cfg file format error at %s\n", str);
		}
		else if(*str == '$')
		{
			assert(special_flag);
			*special_flag = SDT_DOLLAR;
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
	在pattern中查找类型为ET_DELIM的，且对应group为grp的list节点
*/
static INLINE delim_cons_t* search_delim_pat(pattern_t* pat, cfg_group_t* grp)
{
	M_slist* list = pat->cons_head.next;
	delim_cons_t* delim_cons;

	while(list != &pat->cons_head)
	{
		delim_cons = get_delim_p(list);
		if(delim_cons->ele_type == ET_DELIM && delim_cons->grp == grp)
			return delim_cons;
		list = list->next;
	}

	return NULL;
}

static INLINE pattern_t*	new_pat(ne_arg_t* ne_arg)
{
	pattern_t*	pat;
	if( !(pat = (pattern_t*)pi_alloc(sizeof(pattern_t), &ne_arg->pat_pool.valid_pool)) )
		return NULL;

	pat_init(pat);
	if( !(pat->str = (M_sint8*)sp_alloc(sizeof(M_sint8)*FLAG_LEN, &ne_arg->spool)) )
	{
		rt_free(&pat->rt_stub, &ne_arg->pat_pool);
		return NULL;
	}
	return pat;
}

static INLINE void	delete_pat(pattern_t* pat, ne_arg_t* ne_arg)
{
	M_slist			*list, *tmp_list;
	delim_cons_t	*delim_cons;
	str_cons_t		*str_cons;

	list = pat->cons_head.next;
	while(list != &pat->cons_head)
	{
		tmp_list = list->next;

		delim_cons = get_delim_p(list);
		if(delim_cons->ele_type == ET_STRING)
			sp_free(delim_cons, &ne_arg->spool);
		else
			sp_free(delim_cons, &ne_arg->spool);

		list = tmp_list;
	}

	if(pat->str)
		sp_free(pat->str, &ne_arg->spool);
	rt_free(&pat->rt_stub, &ne_arg->pat_pool);
}

/*
	解析配置文件中的分隔符串，并将其插入到radix tree中

	delim_str：	配置文件中的串
	delim_type：分隔符类型，start/end/seg
	grp：		当前delim所属的group。同样的delim，同属于一个group的，在radix tree中只会出现一次
	model：		radix tree的所在
	ne_arg：	各种内存池
	i:			当前group的序号

	返回：0 成功 -1 失败
*/
static INLINE M_sint32	parse_group_delim_string(M_sint8* delim_str, M_sint8 delim_type, cfg_group_t* grp, normalize_engine_t* model, ne_arg_t* ne_arg, M_sint32 i)
{
	M_rt_stub*		pat_stub;
	delim_cons_t*	delim_node;
	special_delim_t*	special_delim;
	pattern_t*		pat;
	M_sint32		special_flag;

	if( !(pat = new_pat(ne_arg)) )
		goto out;

	while( (delim_str = get_delim(delim_str, ',', pat->str, &special_flag, &pat->str_len)) )
	{
		switch(special_flag)
		{
		case SDT_HAT:
		case SDT_DOLLAR:
			assert(delim_type != DT_SEG);

			if( !(special_delim = (special_delim_t*)sp_alloc(sizeof(special_delim_t), &ne_arg->spool)) )
				goto out;
			if(special_flag == SDT_HAT)
				special_delim->delim_type = delim_type | DT_HAT;
			else
				special_delim->delim_type = delim_type | DT_DOLLAR;
			special_delim->ele_type = ET_SPECIAL_DELIM;
			special_delim->grp = grp;
			slist_insert(&model->delim_set[i], &special_delim->ele_stub);
			
			continue;

		default:
			pat->str_len = strlen(pat->str);
			pat->type = PT_DELIM;
			rt_init_node(&pat->rt_stub, pat->str, pat->str_len);

			if( (pat_stub = rt_insert_node(&model->pat_tree, &pat->rt_stub, &ne_arg->extra_arg)) )		//insert fail
			{
				delete_pat(pat, ne_arg);
				pat = container_of(pat_stub, pattern_t, rt_stub);
			}

			//要在pat里增加一个delim_str类型的list节点，先查查看有没有，有就直接用了，没有就新建一个
			if( !(delim_node = search_delim_pat(pat, grp)) )
			{
				if( !(delim_node = (delim_cons_t*)sp_alloc(sizeof(delim_cons_t), &ne_arg->spool)) )
					goto out;

				delim_node_init(delim_node, pat, grp);
				slist_insert(&model->delim_set[i], &delim_node->ele_stub);
			}

			delim_node->delim_type |= delim_type;

			if( acsmAddPattern2(model->acmata.ac_handle, pat->str, pat->str_len, 1, 0, 0, 0, pat, 0) != 0 )
				goto out;
			if( acsmAddPattern2(ne_arg->ac.ac_handle, pat->str, pat->str_len, 1, 0, 0, 0, pat, 0) != 0 )
				goto out;

			rt_process_arg(&ne_arg->pat_pool, &ne_arg->extra_arg);

			if( !(pat = new_pat(ne_arg)) )
				goto out;
			break;
		}
	}
	delete_pat(pat, ne_arg);
	return 0;

out:
	return -1;
}

static INLINE M_sint32	parse_group_delim(cfg_group_t* grp, normalize_engine_t* model, ne_arg_t* ne_arg, M_sint32 i)
{
	if(parse_group_delim_string(grp->start_flag, DT_START, grp, model, ne_arg, i) < 0)
		return -1;

	if(parse_group_delim_string(grp->end_flag, DT_END, grp, model, ne_arg, i) < 0)
		return -1;

	if(parse_group_delim_string(grp->seg_delim, DT_SEG, grp, model, ne_arg, i) < 0)
		return -1;

	return 0;
}

static INLINE M_sint32	parse_delims(normalize_engine_t* model, ne_cfg_t* cfg, ne_arg_t* ne_arg)
{
	M_sint32 i = 0;
	pattern_t*	pat;

	if( !(model->delim_set = (M_slist*)sp_alloc(sizeof(M_slist)*cfg->cfg_group_t_nr_sets, &ne_arg->spool)) )
		goto out;

	//将所有的delim加入到radix tree，以及delim_set中
	for(i=0; i<cfg->cfg_group_t_nr_sets; i++)
	{
		slist_init(&model->delim_set[i]);
		if(parse_group_delim(&cfg->cfg_group_t_cfgs[i], model, ne_arg, i) < 0)
			goto out;
	}
	return 0;

out:
	return -1;
}

static INLINE str_pos_info_t*	new_str_pos_info(ne_arg_t* ne_arg, M_sint32	num)
{
	M_sint32 i = 0;
	str_pos_info_t*	str_pos = sp_alloc(sizeof(str_pos_info_t), &ne_arg->tpool);

	if(!str_pos)
		return NULL;

	str_pos->grp_info = sp_alloc(sizeof(group_pos_info_t)*num, &ne_arg->tpool);

	if(!str_pos->grp_info)
	{
		sp_free(str_pos, &ne_arg->tpool);
		str_pos = NULL;
	}

	for(i=0; i<num; ++i)
	{
		slist_init(&str_pos->grp_info[i].cgrp_head);
		str_pos->grp_info[i].nr_candidates = 0;
	}

	return str_pos;
}

static INLINE M_sint32	get_leading_group(str_pos_info_t* str_pos, ne_cfg_t* cfg, ne_arg_t* ne_arg, normalize_engine_t* model)
{
	M_sint32 ret = -1;
	M_slist* ele_stub;
	M_sint32 i = 0;
	special_delim_t*	sdelim;
	candidate_group_t*	cgrp;

	for(i=0; i<cfg->cfg_group_t_nr_sets; ++i)
	{
		ele_stub = model->delim_set[i].next;
		while(ele_stub != &model->delim_set[i])
		{
			sdelim = container_of(ele_stub, special_delim_t, ele_stub);
			if(sdelim->ele_type == ET_SPECIAL_DELIM)
			{
				if(sdelim->delim_type & DT_HAT)
				{
					if( !(cgrp = (candidate_group_t*)sp_alloc(sizeof(candidate_group_t), &ne_arg->tpool)) )
						goto out;

					cgrp->grp = &cfg->cfg_group_t_cfgs[i];
					slist_insert(&str_pos->grp_info[i].cgrp_head, &cgrp->grp_stub);
					++str_pos->grp_info[i].nr_candidates;
					ret = 0;
				}
			}
			else
				assert(sdelim->ele_type == ET_DELIM);

			//slist_reverse(&str_pos->grp_info->cgrp_head);
			ele_stub = ele_stub->next;
		}
	}
	return ret;
out:
	return -1;
}

/*
	get group, seg info of an input string
*/
/*
	1. 把match result按照匹配位置顺序排列成一个链表，done
	2. 把delim_cons按照group进行组织，就是delim_set？需要么？pattern里面也有这些信息。。。，
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
static INLINE str_pos_info_t* split_string(M_sint8* str, M_sint32 str_len, mat_delim_t* match_delim, ne_cfg_t* cfg, normalize_engine_t* model)
{
	M_slist*	pos_stub = match_delim->delim_head.next;
	mat_pos_t*	mat_pos;
	M_sint32	i = 0;
	delim_cons_t*		delim;
	special_delim_t*	sdelim;
	str_pos_info_t*	str_pos = new_str_pos_info(match_delim->ne_arg, cfg->cfg_common_t_cfgs->nr_groups);

	if(!str_pos)
		return NULL;

	//先确定头
	if( get_leading_group(str_pos, cfg, match_delim->ne_arg, model) < 0 )
		return NULL;

	while(pos_stub != &match_delim->delim_head)
	{
		//对每个出现的delimiter, 检查是否是当前candidate group的end flag
		pos_stub = pos_stub->next;
	}

	return str_pos;
}

/*
	id:		pattern_t 
	data:	mat_res_t

	callback for ac mata.
	return value > 0: break ac processing
				 = 0: continue ac processing

*/
static INLINE M_sint32	delim_matcher(pattern_t *id, void *tree, M_sint32 offset, mat_delim_t *data, void *neg_list)
{
	mat_pos_t* mat_pos = (mat_pos_t*)sp_alloc(sizeof(mat_pos_t), &data->ne_arg->tpool);

	if( !mat_pos )
		return 1;
	
	assert(offset >= 0);
	//printf("matches %s, offset: %d, address: 0x%p\n", id->str, offset, id);

	mat_pos->offset = offset;
	mat_pos->pat = id;
	slist_insert(&data->delim_head, &mat_pos->pos_stub);
	return 0;
}

static void print_delim_match_result(mat_delim_t* match_delim)
{
	mat_pos_t* mat_pos;
	M_slist* pos_stub = match_delim->delim_head.next;

	while(pos_stub != &match_delim->delim_head)
	{
		mat_pos = container_of(pos_stub, mat_pos_t, pos_stub);
		printf("%s, offset: %d, address: 0x%p\n", mat_pos->pat->str, mat_pos->offset, mat_pos);
		pos_stub = pos_stub->next;
	}
}

static void print_delim_set(normalize_engine_t* model, ne_cfg_t* cfg)
{
	M_sint32 nr_grps = cfg->cfg_common_t_cfgs->nr_groups;
	M_sint32 i = 0;
	M_slist* ele_stub;
	delim_cons_t*		delim;
	special_delim_t*	sdelim;
	M_sint8*	x;

	for(i=0; i<nr_grps; i++)
	{
		ele_stub = model->delim_set[i].next;
		while(ele_stub != &model->delim_set[i])
		{
			delim = container_of(ele_stub, delim_cons_t, ele_stub);
			if(delim->ele_type == ET_DELIM)
			{
				if(!strcmp(delim->pat->str,"\n"))
					x = "slash-n";
				else if(!strcmp(delim->pat->str,"\r"))
					x = "slash-r";
				else if(!strcmp(delim->pat->str,"\t"))
					x = "slash-t";
				else
					x = delim->pat->str;
				printf("%s, group: %s, delim_type: 0x%x(1:start,2:end,4:seg,8:hat,0x10:dollar)\n", x, delim->grp->name, delim->delim_type);
			}
			else
			{
				sdelim = container_of(ele_stub, special_delim_t, ele_stub);
				if(sdelim->delim_type & DT_HAT)
					printf("HAT, group: %s, delim type: 0x%x\n", sdelim->grp->name, sdelim->delim_type);
				else if(sdelim->delim_type & DT_DOLLAR)
					printf("DOLLAR, group: %s, delim type: 0x%x\n", sdelim->grp->name, sdelim->delim_type);
				else
					printf("unknown special delim type: 0x%x\n", sdelim->delim_type);
			}

			ele_stub = ele_stub->next;
		}
	}
}

static INLINE void reverse_match_result(mat_delim_t* match_delim)
{
	slist_reverse(&match_delim->delim_head);
}


M_sint32	parse_rules(normalize_engine_t* model, ne_cfg_t* cfg, ne_arg_t* ne_arg)
{
	M_sint32	i = 0;
	M_sint32	state = 0;
	mat_delim_t	match_delim;
	M_sint8*	str;
	M_sint32	str_len;

	slist_init(&match_delim.delim_head);
	match_delim.ne_arg = ne_arg;

	for(i=0; i<cfg->cfg_rule_t_nr_sets; i++)
	{
		str = cfg->cfg_rule_t_cfgs[i].match_rule;
		str_len = strlen(str);
		acsmSearch2(ne_arg->ac.ac_handle, str, str_len, delim_matcher, &match_delim, &state);
		reverse_match_result(&match_delim);

		/*
			这里要做的事情：
			1. 将match_rule分组，分段，得到组边界，段边界
			2. 分析match_rule中的通配符，编号，得到串联的match_rule
			3. 将match_rule中确定的（非通配）字符串加入ac
			4. 分析normal_rule（不需要分组，分段），只需要提取通配，并编号即可
			5. 利用normal_rule的分析结果清理match_rule中不需要的通配编号
			6. 编译ac状态机
		*/
		
		printf("rule: %s\n", cfg->cfg_rule_t_cfgs[i].match_rule);
		print_delim_match_result(&match_delim);
		

		split_string(str, str_len, &match_delim, cfg, model);
		
		ne_arg->tpool.cur_ptr = ne_arg->tpool.pool;
		slist_init(&match_delim.delim_head);
	}
}


M_sint32	build_normalize_engine(normalize_engine_t* model, ne_cfg_t* cfg, M_sint32 memory_size)
{
	M_sint32 i = 0;
	pattern_t*	pat;
	ne_arg_t	ne_arg;

	ne_arg_init(&ne_arg, memory_size);
	ne_model_init(model, &ne_arg);

	if(!ne_arg.ac.ac_handle || !model->acmata.ac_handle)
		goto out;
	
	sort_cfg_group(cfg);

	if( parse_delims(model, cfg, &ne_arg) < 0 )
		goto out;	

	//build ac for delims, it would be used in parse rules: split_string
	if( acsmCompile2(ne_arg.ac.ac_handle, NULL, NULL) < 0 )
		goto out;

	//acsmPrintInfo2(ne_arg.ac.ac_handle);
	//acsmPrintDetailInfo2(ne_arg.ac.ac_handle);
	//acsmPrintSummaryInfo2();

	if( parse_rules(model, cfg, &ne_arg) < 0 )
		goto out;

	print_delim_set(model, cfg);

	acsmFree2(ne_arg.ac.ac_handle);
	free(ne_arg.tpool.pool);
	return ne_arg.spool.cur_ptr - ne_arg.spool.pool;

out:
	if(ne_arg.ac.ac_handle)
		acsmFree2(ne_arg.ac.ac_handle);
	if(model->acmata.ac_handle)
		acsmFree2(model->acmata.ac_handle);

	free(ne_arg.tpool.pool);
	destroy_normalize_engine(model);
	return -1;
}

M_sint32	destroy_normalize_engine(normalize_engine_t* model)
{
	free(model->memory);
}