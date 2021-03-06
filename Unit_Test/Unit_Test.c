// Unit_Test.cpp : 定义控制台应用程序的入口点。
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

#include "UT_MBase.h"

int main()
{
	int ret = 0;
	printf("int:%d, long: %d, long long:%d, ptr: %d\n", sizeof(int), sizeof(long), sizeof(long long), sizeof(void*));

	ret &= UT_radix_mata();
	ret &= UT_radix_multichar_pool();
	ret &= UT_bsthash();
	ret &= UT_radix_tree();
	ret &= UT_radix_tree_pool();
	ret &= UT_tls();
	ret &= UT_err();

	ret &= UT_list();
	ret &= UT_rbtree();
	ret &= UT_slisthash();
	ret &= UT_dlisthash();
	ret &= UT_bsthash();

	ret &= UT_atomic();

	ret &= UT_lqueue();
	ret &= UT_race();
	ret &= UT_data_exchanger();

	ret &= UT_mutex();
	ret &= UT_rwlock();

	ret &= UT_sem();
	return 0;
}

