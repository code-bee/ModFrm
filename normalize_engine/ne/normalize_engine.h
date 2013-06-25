#ifndef __NORMALIZE_ENGINE_H__
#define __NORMALIZE_ENGINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MBase.h"

#include "config.h"
#include "acsmx2.h"

/*
	整体流程
	1. 读入配置
	2. 解析配置，生成便于使用，高效的数据结构
	3. 对输入串进行匹配，提取通配字段，构造归一化结果
*/

/*
	1. 读入配置
*/
#define NR_GROUPS		8
#define GROUP_NAME_LEN	16
#define	FLAG_LEN		16
#define RULE_LEN		256

typedef struct st_cfg_common
{
	M_sint32	nr_groups;
	M_sint32	pattern_id_len;
	M_sint8		default_group[GROUP_NAME_LEN];
	M_sint8		group_order[GROUP_NAME_LEN*NR_GROUPS];
} cfg_common_t;

typedef struct st_cfg_group
{
	M_sint8 name[FLAG_LEN];
	M_sint8 start_flag[FLAG_LEN];
	M_sint8 end_flag[FLAG_LEN];
	M_sint8 seg_delim[FLAG_LEN];
	M_sint32  seg_in_order;
} cfg_group_t;

typedef struct st_cfg_rule
{
	M_sint8 match_rule[RULE_LEN];
	M_sint8 normal_rule[RULE_LEN];
} cfg_rule_t;

/*
	用user_config_set_DECLARE和user_config_set_t定义整体配置变量都可以
	前者的不方便在于变量名由系统给出，不直观
	后者的不方便在于使用时需要做强制类型转换，更麻烦
	相比较之下，推荐前者

	更好的方法是用户自己按照宏展开后的顺序定义变量，这样变量名用户可以自己定义，
	同时沿用第二种方案中的g_config中第二项的写法即可
*/
typedef struct st_ne_cfg
{
	user_config_set_DECLARE(cfg_common_t);
	user_config_set_DECLARE(cfg_group_t);
	user_config_set_DECLARE(cfg_rule_t);
	//宏展开后等价于：
	//cfg_common_t*	cfg_common_t_cfgs;
	//M_sint32			cfg_common_t_nr_sets;
	//cfg_group_t*	cfg_group_t_cfgs;
	//M_sint32			cfg_group_t_nr_sets;
	//cfg_rule_t*	cfg_rule_t_cfgs;
	//M_sint32			cfg_rule_t_nr_sets;

	//另一种定义方式：
	//user_config_set_t common;
	//user_config_set_t group;
	//user_config_set_t rule;
} ne_cfg_t;

M_sint32 read_ne_config(M_sint8* filename, ne_cfg_t* cfgs);
void release_ne_config(ne_cfg_t* cfgs);
void print_cfg(ne_cfg_t* cfgs);

/*
	2. 配置解析，数据对象构造阶段
		共有4个对象需要构造：rule对象，匹配模式（pattern）对象，通配符(wildcard)对象和AC状态机对象

	rule对象：
		目标：	管理匹配模式对象的相对位置关系，以检测待匹配串是否匹配成功
				管理通配符对象的相对位置关系，以从待匹配串中提取通配内容
				管理normal_rule，以生成归一化串

	匹配模式对象：
		目标：	快速从AC状态机的模式匹配结果中获取该结果属于哪个rule对象
				快速从AC状态机的分隔符匹配结果中获取该分隔符的类型

	通配符对象：
		目标：	管理通配符在指定rule中的位置、长度、类型、编号信息，以从待匹配串中提取通配内容

	AC状态机对象：
		目标：	管理所有需要从待匹配串中提取的内容，包括分隔符，匹配模式，以实现一次扫描，提取信息的功能

	以上这些对象都需要在构造阶段构造完毕。
	为了构造这些对象，还需要实现一个方法：
	根据分隔符位置、类型信息对输入串进行组、段划分。输入串可以是match_rule，也可以是待匹配串。

	为各个对象起名字：
	串约束（str_cons)：	匹配规则中按组(group)分割剩下的非通配部分，保存其相对位置信息和要求，存在于rule和串radix tree中
						str_cons在rule中是按照出现顺序组织的
	分隔符约束（delim_cons）：	保存分隔符的类型信息，存在于分隔符链表（delim_list）和串radix中。其中delim_list中的delim_cons
								是按照group顺序组织的。分隔符与串约束的串必不相同
	
	通配符约束（wc_cons）： 通配符约束记录通配符在匹配规则和归一化规则中的位置和编号信息。编号信息反映了提取规则
							通配符类型是静态变量，只有四种类型

	模式串（pattern）：	模式串只代表字符串，与约束无关。str_cons和delim_cons都指向pattern。
	模式串的radix_tree：将pattern组织成radix tree，每个pattern都有约束链表。str_cons和delim_cons挂在同一个链表上。
						由于分隔符与模式串不相同，所以事实上一个pattern的约束链表中要么全是delim_cons，要么全是str_cons

	rule_t：包含匹配规则和归一化规则。都以链表的形式按照顺序存放
*/
struct st_rule;
struct st_pattern;

//typedef struct st_group
//{
//	M_slist			list_stub;		//used in rule_t. all groups are in order
//	//struct st_rule*	rule;
//	cfg_group_t*	cfg_grp;
//} group_t;

typedef cfg_group_t	group_t;

/*
	ele_stub lays in all constraints: str_cons, delim_cons, wc_cons
*/
#define ele_stub_DECLARE\
	/* list of it's owner. for string pattern, it is used by rule_t; */\
	/* for wildcard, it is used by rule_t, together with string pattern to build a completed match rule */\
	/* for delim and special delim, it is used by delim_set */\
	M_slist		ele_stub;\
	/* ET_XXX, indicate type of current element: string constraint, delim constraint, special delim constraint, wildcard... */\
	M_sint8		ele_type

/*
	pattern_stub lays in constraint of string pattern and delimiter
*/
#define pattern_stub_DECLARE\
	/* list of str_cons or delim_cons with same string key, used by pattern_t */\
	M_slist		pat_stub;\
	/* points to current pattern */\
	struct st_pattern*	pat;\
	/* which group it belongs to */\
	group_t*	grp;\
	ele_stub_DECLARE

typedef struct st_str_cons
{
	pattern_stub_DECLARE;		// pat_stub is used in pattern_t, ele_stub is used in rule_t
	M_sint8		string_type;	//ST_XXX, boundary seg is completed or not, pos is equal or greater than
	M_sint8		start_pos;
	M_sint8		end_pos;		//both are seg positions
	struct st_rule*	rule;		//which rule it belongs to
} str_cons_t;

/*
	分隔符可以完全相同，但不允许出现包含，或前后缀关系
*/
typedef struct st_delim_cons
{
	pattern_stub_DECLARE;		// pat_stub used in pattern_t, ele_stub is used in delim_set
	M_sint16	delim_type;		//DT_XXX, group delim(start/end), or seg delim
} delim_cons_t;

typedef struct st_pattern
{
	M_rt_stub	rt_stub;
	M_sint8*	str;
	M_sint16	str_len;
	M_sint16	type;		//PT_XXX, string pattern or delimiter pattern
	M_slist		cons_head;	//list of constraints, could be pattern constraints, or delimiter constraints
} pattern_t;

/*
	wc_cons_t does not have member varible that points to corresponding rule,
	because wildcard is always visited via rule
*/
typedef struct st_wc_cons
{
	ele_stub_DECLARE;	//ele_stub used in rule_t
	M_sint8		type;	//WT_XXX, wildcard of seg or char	
	M_sint16	no;		//0 if it is not numbered
	group_t*	grp;	//which group it belongs to
} wc_cons_t;

typedef struct st_rule
{
	M_slist		match_list;		// list of wildcard and constraints of string pattern of match_rule
	M_slist		normal_list;	// list of wildcard and string pattern of normal rule
} rule_t;

// used in delim_set, its cons type is CT_SPECIAL_DELIM
// while delim type contains DT_HAT/DT_DOLLAR
// special delim does not exist in radix tree
typedef struct st_special_delim
{
	ele_stub_DECLARE;			// ele_stub used in delim_set
	M_sint16	delim_type;
	group_t*	grp;
} special_delim_t;

/*
	wrapper of ac mata, besides ac handle, callbacks for ac are also integrated
*/
typedef struct st_acmata
{
	ACSM_STRUCT2*	ac_handle;
	//callbacks...
} acmata_t;

/*
	top structure, combine all data together
	normalize_engine_t is a read only structure, could be shared among threads
*/
typedef struct st_ne_model
{
	acmata_t	acmata;			//ac mata
	M_rt_stub*	pat_tree;		//radix tree for patterns
	M_slist*	delim_set;		//delimiter constraint list. 
								//actually it points to a list array, each group has a list in the array
								//member types are special_delim_t and delim_cons_t
								//linked via ele_stub， indentified by ele_type
	rule_t*		rules;			//rule array
	M_sint32	nr_rules;		//number of rules
	void*		memory;			//memory block that ne model layes in
} normalize_engine_t;

/*
	M_poolinf is not thread safe, so thread_ne_t is introduced to support multithread scenarios
*/
typedef struct st_thread_ne
{
	normalize_engine_t*	model;
	M_poolinf	delim_pool;		//sizeof(delim_cons_t)
	//M_poolinf	str_pool;		//sizeof(str_cons_t)
	//M_poolinf	pat_pool;		//sizeof(pattern_t)
} thread_ne_t;

/*
	called after config file read over, usually in main thread
	return value of build_normalize_engine: bytes of memory it used if success
											-1 if fail
*/
M_sint32	build_normalize_engine(normalize_engine_t* model, ne_cfg_t* cfg, M_sint32 memory_size);
M_sint32	destroy_normalize_engine(normalize_engine_t* model);
/*
	called in worker thread, after ne model build over
	actually, it is to build memory pool for worker threads
*/
M_sint32	build_thread_model(normalize_engine_t* model, thread_ne_t* t_model);

/*
	destroy thread model does not destory ne_model it wraps..
*/
M_sint32	destory_thread_model(thread_ne_t* t_model);



/*
	3. 待处理串匹配，归一化阶段

	过程：
	1. 用AC状态机对象处理待匹配串，获得各个匹配结果
	2. 初筛匹配结果，删除匹配不完整的规则
	3. 利用分隔符的位置信息对待匹配串进行组、段分割
	4. 进一步检查各匹配约束是否满足，删除不满足匹配约束的规则
	5. 如果还剩下多条规则匹配成功（此时需要给出告警，供业务人员分析），选择优先级最高的规则。
	   优先级内部设定，外部不可见。
	   首先比较通配符的数目，通配符少的优先级高；然后比较通配符的精度，
	   $通配符的精度高于*的通配精度，选择$通配数目多的规则
	   如果还不能选择，不做处理
	6. 找到合适的规则后，提取通配内容，生成归一化串
*/

/*
	to wildcard element, at beginning, both str and str_len are 0
	after processing, they are replaced by pointers to match_string,
	so that a normalized string could be constructed
*/
typedef struct st_rule_ele
{
	M_slist		list_stub;
	M_sint8*	str;
	M_sint16	str_len;
	M_sint16	type;		// normal string element, or wildcard element
} rule_ele_t;

typedef struct st_match_string
{
	M_sint8*	str;
	M_sint32	str_len;
	M_slist		ele_list;
} match_string_t;


#if 0

/*
	2. 解析配置，生成便于使用，高效的数据结构

	a. 分析common组，然后用common组的信息解析group组
		这里需要对各种边界字符进行转义：\t,\r等等
	b. 对rule组的解析放在第3步做

	具体做法：
	将所有的组分隔符和段分隔符组织成一个AC，
	规则串、归一串、待处理串首先都需要通过这个AC得到各个分隔符的位置

	然后利用组规则、段规则解释这些分隔符，确定各个组、段的边界

	组分隔符，段分隔符的两个约束：
	1. 组分隔符与组内段分隔符必不相同
	2. 除了1，分隔符可以相同，但不能存在嵌套关系

	解析AC匹配结果时首先确定各组的边界，然后在组边界内部找段边界。组边界内部出现的非本组的段分隔符直接忽略

*/

/*
	group_t按照配置定义的group顺序组织成链表，同时在链表头上提供default_group的指针
*/
typedef struct st_cfg_group group_t;

typedef struct st_rule
{
	M_slist		list_head;
	group_t*	default_group;
} rule_t;

/*
	type: 记录该分隔符在对应grp中的类型：start/end/share，各占1位
*/
#define	DSTART	0x01
#define DEND	0x02
#define DSHARE	0x04
typedef struct st_group_info
{
	M_slist		list_stub;
	group_t*	grp;
	M_sint32	type;
} group_info_t;


typedef struct st_seg_info
{
	M_slist		list_stub;
	group_t*	grp;
} seg_info_t;

typedef struct st_delim
{
	M_rt_stub	rt_stub;
	M_sint8*	delim;
	M_slist		grp_list;
	M_slist		seg_list;
} delim_t;

typedef struct st_delim_info
{
	M_rt_stub*	delim_root;
} delim_info_t;

typedef struct st_match_str_pos
{
	M_slist		next_pos;
	seg_info_t*	seg;
} match_str_pos_t;

typedef struct st_match_str
{
	M_rt_stub	rt_stub;
	M_slist		;
	M_sint8*	str;
	M_sint32	str_len;
} match_str_t;

typedef struct st_wildchars
{
	
} wildchars_t;



//M_sint32	build_delim_info(ne_cfg_t* cfg, delim_info_t* dinfo);
//void free_delim_info(delim_info_t* dinfo);
#endif

#endif //__NORMALIZE_ENGINE_H__


