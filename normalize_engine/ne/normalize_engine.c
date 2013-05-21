
#include "normalize_engine.h"

config_set_t g_common_config[] = 
{
	{"group_number_in_rule",	int_reader,		offset_of(cfg_common_t, nr_groups_in_rule)},
	{"pattern_id_len",			int_reader,		offset_of(cfg_common_t, pattern_id_len)},
	{"group_order",				string_reader,	offset_of(cfg_common_t, group_order)},
	{"default_group",			string_reader,	offset_of(cfg_common_t, default_group)}
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

static ne_cfg_t	s_cfg;

int read_ne_config(char* filename, ne_cfg_t* cfgs)
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
		printf("nr_groups_in_rule = %d\n", common->nr_groups_in_rule);
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
	printf("%s\n", cfgs->cfg_common_t_cfgs[0].default_group);
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