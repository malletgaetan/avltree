#if defined(TEST) || defined(DEBUG)
# include <assert.h>
# define DEBUG_ASSERT(cond, mes) (assert(cond && mes))
#else
# define DEBUG_ASSERT ((void)0)
#endif

typedef struct avl_node {
	size_t          index;
	struct avl_node *left;
	struct avl_node *right;
	int             height;
} AVLNode;

typedef struct {
	AVLNode *root;
} AVLTree;

void avl_tree_init(AVLTree *tree);
void avl_node_init(AVLNode *node);
AVLNode *avl_find(AVLTree *tree, size_t line);
void avl_insert(AVLTree *tree, AVLNode *value);