#include "tree.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

tree_t *init_tree(int key_size){
	tree_t *_tree = calloc(1, sizeof(tree_t));
	_tree->key_size = key_size;
}
unsigned int tree_get_node_size(){
	return sizeof(tree_node_t);
}

unsigned int tree_get_key_size(void *tree){
	tree_t *_tree = (tree_t *)tree;
	return _tree->key_size;
}

static void*
add_tree_node(tree_t *tree, tree_node_t *node)
{
    tree_node_t *root = NULL, *parent = NULL;
    if(!tree || !node) return NULL;
    if(!tree->root){
        tree->root = node;
        return node;
    }

    root = tree->root;

    while(root){
            parent = root;
            root = (tree_key_compare(node->data, root->data, tree->key_size) < 0) ? root->left : root->right;
    } // while ends

    if(tree_key_compare(node->data, parent->data, tree->key_size) < 0)
        parent->left = node;
    else
        parent->right = node;

    return node;
}



void *tree_add_node(void *ds, void *node){
	return add_tree_node((tree_t*) ds, (tree_node_t *) node);		
}

void* tree_init_node(void *key, unsigned int key_size, unsigned int max_key_size){
	if(!key || !key_size) return NULL;
	if(key_size > max_key_size){
                printf("Error : key size is greater than max_key_size. %s()\n", __FUNCTION__);
		return NULL;
	}
		
	tree_node_t *node = calloc(1, sizeof(tree_node_t));
	void *data = calloc(1, key_size);
	memcpy(data, key , key_size);
	node->data = data;
	return (void *)node;
}


static tree_node_t *
_tree_lookup(tree_node_t *root, void *key, int key_size){
	if(!root) return NULL;
	 if(tree_key_compare(root->data, key, key_size) == 0)
		return root;
	 if(tree_key_compare(root->data, key, key_size) < 0)
		return _tree_lookup(root->right, key, key_size);
	return _tree_lookup(root->left, key, key_size);
}

void* tree_lookup(void *ds, void *key){
	if(!ds || !key) return NULL;
	tree_t *tree = (tree_t *)ds;
	if(!tree->root) return NULL;
	char tree_key[tree->key_size];
	memset(tree_key, 0, tree->key_size);
	memcpy(tree_key, key, tree->key_size);
	return _tree_lookup(tree->root, key, tree->key_size);
}


int tree_key_compare(void *key1, void *key2, int key_size){
	if(*(int *)key1 < *(int *)key2)
		return -1;
	else if(*(int *)key1 == *(int *)key2)
		return 0;
	else
		return 1;
}

void tree_node_free(void *node){
        free(((tree_node_t *)node)->data);
        free(node);
}

