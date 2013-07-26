/* 
	M_tls.c : tls实现。TLS：thread local storage

	kevin.zheng@gmail.com
	2012/12/09
*/

#include "../MBase_priv.h"
#include "MBase.h"


/*
	tls key, identifed by index in key_array of M_tls
	it is used by user to visit thread local variables

	here we use free_t instead of M_free_t, is because we want to be compatible with pthread_key_create.
*/
typedef struct st_tls_key
{
	M_bst_stub*	root;
	free_t	destroyer;
} tls_key_t;

/*
	core varible of M_tls
*/
typedef struct st_tls
{
	tls_key_t*	key_array;		/* key array, user holds these keys, the keys are also roots of rbtrees */
	M_uint16	array_size;		/* size of key_array, maximum is 32768 */
	M_uint16	nodes_count;	/* actual nodes in key array, maximum is 32768 */
	M_atomic	rwlock;			/* rwlock for thread safe */
} M_tls;

/*
	nodes of tree rooted by tls_key_t->root
*/
typedef struct st_tls_node
{
	M_bst_stub	bst_stub;
	M_tid_t		tid;		/* node key */
	M_sint32	color;		/* node color */
	void*		value;		/* node data */
} tls_node_t;

static INLINE M_sintptr	tls_cmp_key(void* key1, void* key2)
{
	return cmp_key_M_uintptr(key1, key2);
}

static INLINE void*		tls_get_key(void* stub)
{
	return &(container_of(stub, tls_node_t, bst_stub)->tid);
}
static INLINE M_sint32	tls_get_rbcolor(void* stub)
{
	return container_of(stub, tls_node_t, bst_stub)->color;
}
static INLINE void		tls_set_rbcolor(void* stub, int color)
{
	container_of(stub, tls_node_t, bst_stub)->color = color;
}

static INLINE void		tls_node_destroyer(void* stub, void* pool)
{
	tls_node_t* node;
	free_t	destroyer = (free_t)pool;

	node = container_of(stub, tls_node_t, bst_stub);
	if(destroyer && destroyer != (void*)(-1))
		destroyer(node->value);
	free(node);
}

static M_tls	s_tls;

#define MAX_TLS_COUNT	32768

M_sint32	M_tls_init()
{
	if(s_tls.key_array)
		return 0;

	s_tls.array_size = 1;
	s_tls.nodes_count = 0;
	atomic_init(&s_tls.rwlock);

	if( (s_tls.key_array = malloc(s_tls.array_size * sizeof(tls_key_t))) )
		return 0;
	else
		return -1;
}

M_sint32	M_tls_destroy()
{
	M_sint32 i = 0;

	atomic_wrlock(&s_tls.rwlock);
	for(i=0; i<s_tls.nodes_count; ++i)
	{
		if(s_tls.key_array[i].root)
			bst_free_all(&s_tls.key_array[i].root, tls_node_destroyer, (void*)s_tls.key_array[i].destroyer);
	}
	atomic_rwunlock(&s_tls.rwlock);
	atomic_destroy(&s_tls.rwlock);

	return 0;
}

M_sint32	M_tls_isinited()
{
	return s_tls.array_size;
}

M_sint32	M_tls_create(M_tls_key* key, free_t destroyer)
{
	tls_key_t* tmp;

	//reach max count of tls variables
	if(s_tls.nodes_count == MAX_TLS_COUNT)
		return -1;

	atomic_wrlock(&s_tls.rwlock);

	if(s_tls.nodes_count == s_tls.array_size)
	{
		if( !(tmp = malloc(sizeof(tls_key_t) * (s_tls.array_size << 1))) )
			return -1;

		memcpy(tmp, s_tls.key_array, sizeof(tls_key_t)*s_tls.nodes_count);
		free(s_tls.key_array);
		s_tls.key_array = tmp;
		s_tls.array_size <<= 1;
	}

	*key = s_tls.nodes_count++;
	tmp = &(s_tls.key_array[*key]);
	atomic_rwunlock(&s_tls.rwlock);

	tmp->root = NULL;
	tmp->destroyer = destroyer;

	return 0;
}

M_sint32	M_tls_delete(M_tls_key key)
{
	M_size_t	index = (M_size_t)key;
	tls_key_t*	kt;

	if(index < 0 || index > s_tls.nodes_count)	
		return -1;

	atomic_wrlock(&s_tls.rwlock);
	
	kt = (tls_key_t*)s_tls.key_array + index;
	if(!kt->root)
	{
		atomic_rwunlock(&s_tls.rwlock);
		return -1;
	}

	bst_free_all(&(kt->root), tls_node_destroyer, (void*)kt->destroyer);
	kt->destroyer = (free_t)(-1);

	atomic_rwunlock(&s_tls.rwlock);
	
	return 0;
}
void*		M_tls_get(M_tls_key key)
{
	tls_key_t*	kt;
	M_tid_t tid = M_get_tid();
	M_bst_stub* stub;

	atomic_rdlock(&s_tls.rwlock);

	kt = (tls_key_t*)s_tls.key_array + (M_size_t)key;
	if(kt->destroyer == (void*)-1)
	{
		atomic_rwunlock(&s_tls.rwlock);
		return NULL;
	}
	stub = bst_search(kt->root, &tid, tls_cmp_key, tls_get_key);
	
	atomic_rwunlock(&s_tls.rwlock);

	return stub ? container_of(stub, tls_node_t, bst_stub)->value : NULL;
}
M_sint32	M_tls_set(M_tls_key key, void* data)
{
	tls_key_t*	kt;
	M_tid_t	tid = M_get_tid();
	M_bst_stub*	stub;
	tls_node_t*	node;

	atomic_rdlock(&s_tls.rwlock);

	kt = (tls_key_t*)s_tls.key_array + (M_size_t)key;
	if(kt->destroyer == (void*)-1)
	{
		atomic_rwunlock(&s_tls.rwlock);
		return -1;
	}
	
	if( (stub = bst_search(kt->root, &tid, tls_cmp_key, tls_get_key)) )
	{
		node = container_of(stub, tls_node_t, bst_stub);
		if(kt->destroyer)
			kt->destroyer(node->value);

		node->value = data;
	}
	else
	{
		atomic_rwunlock(&s_tls.rwlock);

		node = malloc(sizeof(tls_node_t));
		node->value = data;
		node->tid = tid;
		
		atomic_wrlock(&s_tls.rwlock);
		kt = (tls_key_t*)s_tls.key_array + (M_size_t)key;	//kt needs to be re-gotten, 
															//because key_array may be moved during previous unlock period
		rbt_insert_node(&(kt->root), &(node->bst_stub), tls_cmp_key, tls_get_key, tls_get_rbcolor, tls_set_rbcolor);
	}

	atomic_rwunlock(&s_tls.rwlock);
	
	return 0;
}
