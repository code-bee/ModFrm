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

#define	WT_SINGLECHAR	0
#define	WT_SINGLESEG	1
#define	WT_MULTICHAR	2
#define	WT_MULTISEG		3
#define	NR_WILDCARD		4

typedef struct st_cfg_common
{
	M_sint8		default_group[GROUP_NAME_LEN];
	M_sint8		group_order[GROUP_NAME_LEN*NR_GROUPS];
	M_sint8		wildcard[NR_WILDCARD][FLAG_LEN];
	M_sint8		pattern_id_len;
	M_sint8		escape_char;
	M_sint8		nr_groups;
	M_sint8		group_delim_reuse;
	M_sint8		flow_mode;			
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
typedef cfg_group_t	group_t;

/*
	分隔符可以完全相同，但不允许出现包含，或前后缀关系
*/
//typedef struct st_delim_info
//{
//	M_slist		delim_stub;		//used by normalize_engine_t->delim_set
//	M_sint16*	delim_type;		//delim type array, each group has an entry corresponding to group id
//								//DT_XXX, only DT_START/DT_END/DT_SEG is valid in pattern_t, 
//								//DT_HAT/DT_DOLLAR does not exist in pattern_t
//} delim_info_t;

typedef struct st_pattern
{
	//M_rt_stub	rt_stub;
	M_sint8*	str;
	M_sint16*	delim_type;		//not NULL if type == PT_DELIM
								//delim type array, each group has an entry corresponding to group id
								//DT_XXX, only DT_START/DT_END/DT_SEG is valid in pattern_t, 
								//DT_HAT/DT_DOLLAR does not exist in pattern_t
	M_sint16	str_len;
	M_sint16	type;			//PT_XXX, string pattern or delimiter pattern
	
	//delim_info_t	delim_info;	//delimiter info. only valid if type == PT_DELIM
} pattern_t;

/*
	each wildcard in match rule is descripted by a wc_info_t structure
	because wildcard absorbs 1 delimiter at most, the delimiter it absorbs also list here
*/
struct st_rm_wc_info;
typedef struct st_wc_info
{
	struct st_rm_wc_info*	rm_wc_info;
	pattern_t*		wc_pat;
	M_sint32		pos;			//char position relative to whole match rule
	M_sint16		seg_pos;		//seg positions relative to current group	
	M_sint8			wc_seq;			//sequence id of wildcard in match_rule
	M_sint8			nm_wc_seq;		//sequence id of wildcard in normal_rule
	M_sint8			grp;			//which group current wildcard belongs to
	M_sint8			cmp_type;		//CT_XXX, start pos is greater or equle to start_pos
	M_sint8			wc_type;		//WT_XXX, single char, single seg, multi char, multi seg...
	M_sint8			wc_len;			//length of current wildcard, including sequence id
} wc_info_t;

typedef struct st_delim_pos
{
	M_bst_stub	rbt_stub;
	M_dlist		list_stub;		//used by mat_delim_t->delim_head
	pattern_t*	delim_pat;		//delimiter info, lays in pat_tree
	M_sint32	pos;			//char pos, key
	M_sint16	seg_pos;		//seg pos relative to current group
	M_sint8		grp_id;			//index of group that it belongs to
	M_sint8		color;
} delim_pos_t;

typedef struct st_delim_info
{
	pattern_t*	delim_pat;
	M_sint32	pos;			//char pos, key
	M_sint16	seg_pos;		//seg pos relative to current group
	M_sint8		grp_id;			//index of group that it belongs to
} delim_info_t;

typedef struct st_rm_wc_info
{
	M_sint16		nr_wcs;			// number of wildcards in wc_info
	M_sint16		nr_delims;
	wc_info_t*		wc_info;		// no memory allocated, points to some offset of rule_t->ori_wc_arr
	delim_info_t*	delim_info;		// dynamic allocated memory from ne_arg->spool
} rm_wc_info_t;

typedef struct st_normal_info
{
	M_slist		nm_stub;		//used by rule_t->nm_head
	M_sint8*	str;			//string segment, part of rule_t->normal_rule
	M_sint32	str_len;		//length of string segment
	wc_info_t*	next_wc;		//nm_wc just after str. if normal_info is lead by wildcard, str of first normal_info is NULL
} normal_info_t;

typedef struct st_rule
{
	M_sint32		nr_ori_wc;		//number of wildcards in match_rule
	M_sint32		nr_rm_wc;		//number of wildcards in rm_rule
	wc_info_t*		ori_wc_arr;		//array of ori wildcard, has nr_ori_wc entries
	rm_wc_info_t*	rm_wc_arr;		//wc array, has nr_rm_wc entries, for looking nr_wc up in match_rules
	M_slist			nm_head;		//info of normal rule
	M_sint8*		match_rule;		//match rule string
	M_sint8*		normal_rule;	//normal rule string
} rule_t;

typedef struct st_acmata
{
	ACSM_STRUCT2*	ac_handle;
	//callbacks...
} acmata_t;

/*
	top structure, combine all data together
	normalize_engine_t is a read only structure, could be shared among threads
*/
typedef struct st_normalize_engine
{
	acmata_t	acmata;			//ac mata
	
	M_rm_root	rm_tree;
	M_sint8*	head_grps;		//group array, 1 means corresponding group can be head-group, 0 otherwise, only valid when flow_mode is false
	M_sint8*	tail_grps;		//group array, 1 means corresponding group can be tail-group, 0 otherwise. 
	rule_t*		rules;			//rule array
	M_sint32	nr_rules;		//number of rules
	void*		memory;			//memory block that ne model layes in
	M_sint8		default_grp;	//index of default group
	M_sint8		disorder_seg;	//1 if exists seg without order, 0 otherwise
	M_sint16	rm_depth;		//depth of radix mata tree
} normalize_engine_t;

/*
	called after config file read over, usually in main thread
	memory_size: in and out. [in]: how many memory will be allocated for normalize engine
							 [out]: how many memory is acturally used
	tmp_memory_size: out.	 [out]: how many temporary memory is acturally used

	size of temporary memory is same with  memory_size
*/
normalize_engine_t*	build_normalize_engine(ne_cfg_t* cfg, M_sint32* memory_size, M_sint32* tmp_memory_size);
M_sint32	destroy_normalize_engine(normalize_engine_t* model);


typedef struct st_match_ac
{
	// 这一组三个变量为通用变量，一次初始化，后面无需再变动
	M_stackpool tpool;			
	normalize_engine_t*	model;
	ne_cfg_t*	cfg;

	// 这一组三个变量内部在匹配AC时使用。初始化时为pattern_arr分配内存，之后只修改值
	M_dlist		pat_head;		// 记录AC匹配时各string pattern的信息，沿用delim_pos_t结构
	M_dlist		delim_head;		// 记录AC匹配时各分隔符的位置信息
	M_sint32	nr_pos;			// pattern_arr的有效长度
	pattern_t**	pattern_arr;	// rm匹配时构造的数组。数组长度为model->rm_depth

	M_sint8*	back_mem;		// 记录以上变量初始化完毕后tpool的位置，便于回退。也在初始化时设定
	
	// 这一组变量在每次查询时都需要调整，归一化结果在dst_str中返回，匹配的规则在matched_rule中返回，状态在status中返回
	// 如果正常，status为0；如果不能匹配，status为-1；如果多个匹配，status为成功匹配的数目，且matched_rule和dst_str_len
	// 都记录匹配成功的规则编号
	M_sint8*	src_str;
	M_sint32	src_str_len;
	M_sint8*	dst_str;
	M_sint32	dst_str_len;
	M_sint32	matched_rule;
	M_sint32	status;
} match_ac_t;

match_ac_t*	create_match_arg(normalize_engine_t* model, ne_cfg_t* cfg, M_sint32 memory_size);
void		set_match_arg(match_ac_t* match_ac, M_sint8* src_str, M_sint32 src_str_len);

/*
	返回match_ac->status
*/
M_sint32	normalize_string(match_ac_t* match_ac);

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


