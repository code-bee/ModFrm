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
	M_sint32	nr_groups_in_rule;
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
	owner_stub lays in string pattern, delimiter, and wildcard
*/
#define owner_stub_DECLARE\
	/* list of it's owner. for string pattern, it is used by rule_t; for delimiter, it is used by delim_head */\
	/* for wildcard, it is used by rule_t, together with string pattern to build a completed match rule */\
	M_slist		owner_stub;\
	/* indicate current constraint is of string pattern or delimiter pattern, or wildcard */\
	M_sint8		self_type

/*
	pattern_stub lays in constraint of string pattern and delimiter
*/
#define pattern_stub_DECLARE\
	/* list of constraints of string pattern or delimiters with same string key, used in pattern_t */\
	M_slist		pat_stub;\
	/* points to current pattern */\
	struct st_pattern*	pat;\
	/* which group it belongs to */\
	group_t*	grp;\
	owner_stub_DECLARE

typedef struct st_cons_str_pat
{
	pattern_stub_DECLARE;
	M_sint8		string_type;	//boundary seg is completed or not, pos is equal or greater than
	M_sint8		start_pos;
	M_sint8		end_pos;		//both are seg positions
	struct st_rule*	rule;		//which rule it belongs to
} cons_str_pat_t;

/*
	分隔符可以完全相同，但不允许出现包含，或前后缀关系
*/
typedef struct st_cons_delim_pat
{
	pattern_stub_DECLARE;
	M_sint16	delim_type;		//group delim(start/end), or seg delim
} cons_delim_pat_t;

typedef struct st_pattern
{
	M_rt_stub	rt_stub;
	M_sint8*	str;
	M_sint16	str_len;
	M_sint16	type;		//string pattern or delimiter pattern
	M_slist		cons_list;	//list of constraints, could be pattern constraints, or delimiter constraints
} pattern_t;

typedef struct st_wildcard
{
	owner_stub_DECLARE;
	M_sint8		type;	//wildcard of seg or char	
	M_sint16	no;		//0 if it is not numbered
	M_slist*	ante;	//anterior element in match_rule, could be cons of string pattern, or another wildcard
	M_slist*	post;	//posterior element in match_rule, could be cons of string pattern, or another wildcard
} wildcart_t;

/*
	wrapper of ac mata, besides ac handle, callbacks for ac are also integrated
*/
typedef struct st_acmata
{
	ACSM_STRUCT2*	ac_handle;
	//callbacks...
} acmata_t;

typedef struct st_rule
{
	M_slist		match_list;		// list of wildcard and constraints of string pattern of match_rule
	M_slist		normal_list;	// list of wildcard and string pattern of normal rule
} rule_t;

/*
	top structure, combine all data together
*/
typedef struct st_ne_model
{
	acmata_t	acmata;			//ac mata
	M_rt_stub*	pat_tree;		//radix tree for patterns
	M_slist		delim_head;		//delimiter list
	rule_t*		rules;			//rule array
	M_sint32	nr_rules;		//number of rules
} ne_model_t;

/*
	a helper structure for recording position of delimiters,
	is used to parse input string
*/
typedef struct st_delim_pos
{

} delim_pos_t;

M_sint32	parse_string(M_sint8* str, M_sint32 str_len, delim_pos_t* delim_pos);

M_sint32	build_ne_model(ne_model_t* model, ne_cfg_t* cfg);

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


