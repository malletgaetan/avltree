#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#include "avltree.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline int _height(AVLNode *node)
{
	return node == NULL ? 0 : node->height;
}

static AVLNode *_right_rotate(AVLNode *old_root)
{
	DEBUG_ASSERT(old_root != NULL, "_right_rotate AVLNode shouldn't be NULL");
	AVLNode *new_root = old_root->left;

	old_root->left = new_root->right;
	new_root->right = old_root;

	old_root->height = MAX(_height(old_root->left), _height(old_root->right)) + 1;
	new_root->height = MAX(_height(new_root->left), _height(new_root->right)) + 1;

	return new_root;
}

static AVLNode *_left_rotate(AVLNode *old_root)
{
	DEBUG_ASSERT(old_root != NULL, "_left_rotate AVLNode shouldn't be NULL");
	AVLNode *new_root = old_root->right;

	old_root->right = new_root->left;
	new_root->left = old_root;

	old_root->height = MAX(_height(old_root->left), _height(old_root->right)) + 1;
	new_root->height = MAX(_height(new_root->left), _height(new_root->right)) + 1;

	return new_root;
}

static AVLNode *_insert(AVLNode *node, AVLNode *value)
{
	if (node == NULL) {
		value->height = 1;
		return value;
	}

	if (value->index < node->index) {
		node->left = _insert(node->left, value);
	} else {
		node->right = _insert(node->right, value);
	}

	int left_height = _height(node->left);
	int right_height = _height(node->right);
	int balance = left_height - right_height;
	node->height = MAX(left_height, right_height) + 1;

	if (balance > 1) {
		if (_height(node->left->left) < _height(node->left->right)) {
			node->left = _left_rotate(node->left);
		}
		return _right_rotate(node);
	}

	if (balance < -1) {
		if (_height(node->right->right) < _height(node->right->left)) {
			node->right = _right_rotate(node->right);
		}
		return _left_rotate(node);
	}

	return node;
}

void avl_insert(AVLTree *tree, AVLNode *value)
{
	DEBUG_ASSERT(tree != NULL, "avl_insert AVLTree shouldn't be NULL");
	tree->root = _insert(tree->root, value);
	return;
}

static AVLNode *_find_min(AVLNode *node)
{
	AVLNode *current = node;
	while (current && current->left != NULL) {
		current = current->left;
	}
	return current;
}

AVLNode *avl_find(AVLTree *tree, size_t line)
{
	DEBUG_ASSERT(tree != NULL, "avl_find AVLTree shouldn't be NULL");
	AVLNode *node = tree->root;
	while (node != NULL && node->index != line) {
		node = line < node->index ? node->left : node->right;
	}
	return node;
}

void avl_node_init(AVLNode *node)
{
	DEBUG_ASSERT(node != NULL, "avl_node_init AVLNode shouldn't be NULL");
	node->right = NULL;
	node->left = NULL;
}

void avl_tree_init(AVLTree *tree)
{
	DEBUG_ASSERT(tree != NULL, "avl_init AVLTree shouldn't be NULL");
	tree->root = NULL;
	return;
}

// #ifdef TEST
#define TEST_ARRAY_SIZE 1000000
#include <stdio.h>

void *xmalloc(size_t size)
{
	void *ret = malloc(size);
	assert(ret != NULL);
	return ret;
}

size_t test_fill_tree(int *tree_arr, AVLNode *nodes)
{
	assert(tree_arr[0] != -1);
	size_t i = 1;
	size_t height = 0;

	nodes[0].index = tree_arr[0];
	if (tree_arr[i] == -1) {
		nodes[0].left = NULL;
		i += 1;
	} else {
		size_t j = i;
		i += test_fill_tree(tree_arr + j, nodes + j);
		height = MAX(height, nodes[j].height);
		nodes[0].left = &nodes[j];
	}

	if (tree_arr[i] == -1) {
		nodes[0].right = NULL;
		i += 1;
	} else {
		size_t j = i;
		i += test_fill_tree(tree_arr + j, nodes + j);
		height = MAX(height, nodes[j].height);
		nodes[0].right = &nodes[j];
	}
	nodes[0].height = height + 1;
	return i;
}

AVLNode *test_generate_tree(int *tree_arr, size_t size)
{
	AVLNode *nodes = xmalloc(sizeof(AVLNode) * size);

	test_fill_tree(tree_arr, nodes);

	return nodes;
}

int test_equals(AVLNode *a, AVLNode *b)
{
	if (a == NULL || b == NULL) {
		return a == b;
	}

	if (a->height != b->height || a->index != b->index) {
		return 0;
	}

	return test_equals(a->left, b->left) && test_equals(a->right, a->right);
}

void test_right_rotate(void)
{
	//          10
	//         /  \
	//        5    15
	//       / \
	//      3   7
	//     /
	//    1
	int unbalanced_tree[] = {10, 5, 3, 1, -1, -1, -1, 7, -1, -1, 15, -1, -1};
	AVLNode *unode =
		test_generate_tree(unbalanced_tree, sizeof(unbalanced_tree) / sizeof(unbalanced_tree[0]));
	unode = _right_rotate(unode);

	//          5
	//         / \
	//        3  10
	//       /   / \
	//      1    7 15
	int balanced_tree[] = {5, 3, 1, -1, -1, -1, 10, 7, -1, -1, 15, -1, -1};
	AVLNode *bnode =
		test_generate_tree(balanced_tree, sizeof(balanced_tree) / sizeof(balanced_tree[0]));
	assert(test_equals(unode, bnode) && "right rotate should.. right rotate?");
}

void test_left_rotate(void)
{
	//           5
	//         /  \
	//        1    10
	//            /  \
	//           8   12
	//                 \
	//                  15
	int unbalanced_tree[] = {5, 1, -1, -1, 10, 8, -1, -1, 12, -1, 15, -1, -1};
	AVLNode *unode =
		test_generate_tree(unbalanced_tree, sizeof(unbalanced_tree) / sizeof(unbalanced_tree[0]));
	unode = _left_rotate(unode);

	//          10
	//         /  \
	//        5    12
	//       /  \   \
	//      1   8   15
	int balanced_tree[] = {10, 5, 1, -1, -1, 8, -1, -1, 12, -1, 15, -1, -1};
	AVLNode *bnode =
		test_generate_tree(balanced_tree, sizeof(balanced_tree) / sizeof(balanced_tree[0]));
	assert(test_equals(unode, bnode) && "left_rotate should.. left rotate?");
}

void test_stress(size_t size)
{
	AVLNode *nodes = malloc(sizeof(AVLNode) * size);
	AVLTree tree;
	avl_tree_init(&tree);
	for (size_t i = 0; i < size; i++) {
		avl_node_init(&nodes[i]);
		nodes[i].index = i;
		avl_insert(&tree, &nodes[i]);
		assert(avl_find(&tree, i) != NULL && "inserted data should be retrievable");
		assert(tree.root->height - 1 <= (int)ceil(log2f((float)MAX(2, i))) &&
			   "tree should stay balanced");
	}
}

int main(void)
{
	test_left_rotate();
	test_right_rotate();
	test_stress(TEST_ARRAY_SIZE);
	printf("TEST OK (*˘︶˘*).｡*♡\n");
}

// #endif