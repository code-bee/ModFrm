// normalize_engine.cpp : 定义控制台应用程序的入口点。
//

#if (defined _WINDOWS) || (defined WIN32)
#ifdef _DEBUG
	#pragma comment(lib, "../debug/MBase.lib")
	#pragma comment(lib, "../debug/pthread.lib")
#else
	#pragma comment(lib, "../release/MBase.lib")
	#pragma comment(lib, "../release/pthread.lib")
#endif
#endif

#include "normalize_engine.h"

#define MEM_LEAK
//ne_cfg_t	g_cfg;

struct ac_test
{
	M_dlist list_stub;
	M_sint32 offset;
	M_sint32 len;
};

M_sint32	test_matcher(void *id, void *tree, M_sint32 offset, M_dlist* list_head, void *neg_list)
{
	struct ac_test* list_stub = malloc(sizeof(struct ac_test));
	struct ac_test* current;
	M_dlist* tmp_stub;

	list_stub->offset = offset;
	list_stub->len = (int)id;
	dlist_append(list_head, &list_stub->list_stub);
	current = list_stub;
	//printf("add %d* at %d\n", list_stub->len, offset);

	tmp_stub = list_stub->list_stub.prev;
	while(tmp_stub != list_head)
	{
		list_stub = container_of(tmp_stub, struct ac_test, list_stub);
		tmp_stub = tmp_stub->prev;

		if(list_stub->offset >= offset)
		{
			//printf("remove %d* at %d\n", list_stub->len, list_stub->offset);
			dlist_remove(list_head, &list_stub->list_stub);
			free(list_stub);
		}
		else if(list_stub->offset + list_stub->len > offset)
		{
			//printf("remove current %d* at %d\n", current->len, current->offset);
			dlist_remove(list_head, &current->list_stub);
			free(current);
			break;
		}
		else
			break;
	}

	return 0;
}

void test_ac()
{
	char* str = "abcdAbCD1cde11xyz111de111111ef";
	M_dlist	list_head;
	M_dlist* tmp_stub;
	struct ac_test* node;
	M_sint32	state = 0;
	ACSM_STRUCT2* ac = acsmNew2(NULL, NULL, NULL);
	acsmAddPattern2(ac, "1", 1, 1, 0, 0, 0, 1, 0);
	acsmAddPattern2(ac, "abcd", 4, 0, 0, 0, 0, 4, 0);
	acsmAddPattern2(ac, "11", 2, 1, 0, 0, 0, 2, 0);
	acsmAddPattern2(ac, "111", 3, 1, 0, 0, 0, 3, 0);	

	dlist_init(&list_head);
	acsmCompile2(ac, NULL, NULL);
	Print_DFA(ac);
	acsmSearch2(ac, str, strlen(str), test_matcher, &list_head, &state);

	acsmFree2(ac);

	tmp_stub = list_head.next;
	while(tmp_stub != &list_head)
	{
		node = container_of(tmp_stub, struct ac_test, list_stub);
		tmp_stub = tmp_stub->next;

		printf("%d* at %d\n", node->len, node->offset);
		free(node);
	}
}

void print_normal_result(match_handle_t* match_handle)
{
#ifdef _DEBUG_PRINT
	M_sint8*	normal_result;
	M_sint32	normal_result_len;
	rule_t*		matched_rule;
	M_sint32	nr_match_rules;
	M_sint32	i;

	if( (nr_match_rules = get_normalize_rule_count(match_handle)) >= 1)
	{
		for(i=0; i<nr_match_rules; ++i)
		{
			normal_result = get_normalize_string(match_handle, i, &matched_rule, &normal_result_len);
			printf("normal string: %s\n", normal_result);
		}
	}
	else
		printf("match fail\n");
	printf("\n");
#endif
}

int main(int argc, char* argv[])
{
	ne_cfg_t	cfg;
	normalize_engine_t*	model;
	M_sint8*	key = "/";
	M_sint32	key_len = 0;
	M_sint32	match_len;
	M_rt_stub*	rt_stub;
	pattern_t*	pat;
	M_slist*	list_stub;
	M_sint32	i;
	M_sint32	memsize = 1024*1024, tmp_memory_size;
	match_handle_t*	ac_arg;

#ifdef __M_CFG_OS_LINUX
	read_ne_config("normalize_engine.cfg", &cfg);
#else
	read_ne_config("..\\normalize_engine\\ne\\normalize_engine.cfg", &cfg);
#endif
	//read_ne_config("..\\normalize_engine\\ne\\a.txt", &cfg);
	print_cfg(&cfg);


	if(model = build_normalize_engine(&cfg, &memsize, &tmp_memory_size))
	{
		for(i = 0; i<1; i++)
		{
			ac_arg = create_match_handle(model, &cfg, 1024*10);

			set_match_handle(ac_arg, "xyz1", strlen("xyz1"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "item.taobao.com/aa/item.htm?id=bb", strlen("item.taobao.com/aa/item.htm?id=bb"));		
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "item.taobao.com/aa/item.htm?y=aa&id=bb&z=cc", strlen("item.taobao.com/aa/item.htm?y=aa&id=bb&z=cc"));		
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "item.taobao.com?id=bb", strlen("item.taobao.com?id=bb"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "item.taobao.comx/aa/item.htm?y=aa&id=bb&z=cc", strlen("item.taobao.comx/aa/item.htm?y=aa&id=bb&z=cc"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "item.taobao.com?y=aa&id=bb&z=cc", strlen("item.taobao.com?y=aa&id=bb&z=cc"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "item.taobao.comx/item.htm?a=aa&y=aa&id=bb&z=cc", strlen("item.taobao.comx/item.htm?a=aa&y=aa&id=bb&z=cc"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "www.search.suning.com/emall/strd.do?di=1000&ci=100&ai=2", strlen("www.search.suning.com/emall/strd.do?di=1000&ci=100&ai=2"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "**xay.taobao.com/lottery/33/33/adf/ss/33/a/xyz3/1sssabc_dd.htm?xyz=3&sz=3&ids=1&z=*2", strlen("**xay.taobao.com/lottery/33/33/adf/ss/33/a/xyz3/1sssabc_dd.htm?xyz=3&sz=3&ids=1&z=*2"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "*.taobao.com/lottery/adf/a/a/a/xyzxxx/1abc_x.htm?xxx=1&ids=1&sss=222&sz=2", strlen("*.taobao.com/lottery/adf/a/a/a/xyzxxx/1abc_x.htm?xxx=1&ids=1&sss=222&sz=2"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "xyz.suning.com/abc/item.htm?xyz=1", strlen("xyz.suning.com/abc/item.htm?xyz=1"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "xyz.taobao.com/abc/item.htm?id=1", strlen("xyz.taobao.com/abc/item.htm?id=1"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			set_match_handle(ac_arg, "**xay.taobao.com/lottery/33/33/adf/ss/33/a/xyz3/1sssabc_dd.htm?xyz=3&sz=3&ids=1&m=3&n=4&f=5&z=*2", strlen("**xay.taobao.com/lottery/33/33/adf/ss/33/a/xyz3/1sssabc_dd.htm?xyz=3&sz=3&ids=1&m=3&n=4&f=5&z=*2"));
			normalize_string(ac_arg);
			print_normal_result(ac_arg);

			destroy_match_handle(ac_arg);
		}

		destroy_normalize_engine(model);
		//printf("%d\n", memsize);
		memsize = 1024*1024;
		
	}

	match_len = pi_get_mem_cts();
	
	release_ne_config(&cfg);
	

//	test_ac();
	return 0;
}

