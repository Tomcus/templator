#ifndef TEMPLATOR_AVL_TREE_H
#define TEMPLATOR_AVL_TREE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*TemplatorAVLGetKey)(void*);
typedef int (*TemplatorAVLCompareKey)(void*, void*);
typedef void (*TemplatorAVLFreeNode)(void*);

typedef struct {
    size_t valueSize;
    TemplatorAVLGetKey getKey;
    TemplatorAVLCompareKey compareKeys;
    TemplatorAVLFreeNode freeNodeData;
} TemplatorAVLTreeConfig;

typedef struct Node {
    struct Node* parrent;
    struct Node* left;
    struct Node* right;
    uint16_t depth;
    uint8_t dataAnchror;
} TemplatorAVLNode;

typedef struct {
    TemplatorAVLTreeConfig* config;
    TemplatorAVLNode* root;
} TemplatorAVLTree;

void templator_avl_tree_init(TemplatorAVLTree* tree, TemplatorAVLTreeConfig* config);
void templator_avl_tree_free(TemplatorAVLTree* tree);

void* templator_avl_tree_insert_with_key_hint(TemplatorAVLTree* tree, void* keyHint);
void* templator_avl_tree_get_with_key_hint(TemplatorAVLTree* tree, void* keyHint);

void templator_avl_tree_bubble_up(TemplatorAVLTree* tree, TemplatorAVLNode* node);
void templator_avl_tree_update_depth(TemplatorAVLNode* node);

TemplatorAVLNode* templator_avl_tree_rotate_left(TemplatorAVLNode* node);
TemplatorAVLNode* templator_avl_tree_rotate_right(TemplatorAVLNode* node);

TemplatorAVLNode* templator_avl_tree_right_right_rotate(TemplatorAVLNode* node);
TemplatorAVLNode* templator_avl_tree_right_left_rotate(TemplatorAVLNode* node);
TemplatorAVLNode* templator_avl_tree_left_right_rotate(TemplatorAVLNode* node);
TemplatorAVLNode* templator_avl_tree_left_left_rotate(TemplatorAVLNode* node);

TemplatorAVLNode* templator_avl_tree_alloc_node(TemplatorAVLTreeConfig* config, TemplatorAVLNode* parrent);
void templator_avl_tree_free_node(TemplatorAVLNode* node, TemplatorAVLTreeConfig* config);

void* templator_avl_tree_helper_identity(void* self);
void templator_avl_tree_helper_nop(void* self);

#ifdef __cplusplus
}
#endif

#endif // TEMPLATOR_AVL_TREE_H
