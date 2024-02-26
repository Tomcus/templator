#include "templator/avl_tree.h"

#include <stdlib.h>

void templator_avl_tree_init(TemplatorAVLTree* tree, TemplatorAVLTreeConfig* config) {
    tree->config = config;
    tree->root = NULL;
}

void templator_avl_tree_free(TemplatorAVLTree* tree) {
    if (tree->root != NULL) {
        templator_avl_tree_free_node(tree->root, tree->config);
        tree->root = NULL;
    }
}

TemplatorAVLNode* templator_avl_tree_alloc_node(TemplatorAVLTreeConfig* config, TemplatorAVLNode* parrent) {
    TemplatorAVLNode* node = malloc(3 * sizeof(int*) + sizeof(uint16_t) + sizeof(uint8_t) + config->valueSize);
    node->parrent = parrent;
    node->left = node->right = NULL;
    node->depth = 1;
    return node;
}

void templator_avl_tree_free_node(TemplatorAVLNode* node, TemplatorAVLTreeConfig* config) {
    if (node->left != NULL) {
        templator_avl_tree_free_node(node->left, config);
    }
    if (node->right != NULL) {
        templator_avl_tree_free_node(node->right, config);
    }
    if (config->freeNodeData != NULL) {
        config->freeNodeData(&node->dataAnchror);
    }
    free(node);
}

void* templator_avl_tree_insert_with_key_hint(TemplatorAVLTree* tree, void* keyHint) {
    if (tree->root == NULL) {
        tree->root = templator_avl_tree_alloc_node(tree->config, NULL);
        return &tree->root->dataAnchror;
    }

    TemplatorAVLNode* node = tree->root;
    int res;
    while ((res = tree->config->compareKeys(keyHint, tree->config->getKey(&node->dataAnchror))) != 0) {
        if (res < 0 && node->left != NULL) {
            node = node->left;
        } else if (res > 0 && node->right != NULL) {
            node = node->right;
        } else if (res < 0) {
            node->left = templator_avl_tree_alloc_node(tree->config, node);
            void* to_ret = &node->left->dataAnchror;
            templator_avl_tree_bubble_up(tree, node);
            return to_ret;
        } else if (res > 0) {
            node->right = templator_avl_tree_alloc_node(tree->config, node);
            void* to_ret = &node->right->dataAnchror;
            templator_avl_tree_bubble_up(tree, node);
            return to_ret;
        }
    }
    return NULL;
}

void* templator_avl_tree_get_with_key_hint(TemplatorAVLTree* tree, void* keyHint) {
    if (tree->root == NULL) {
        return NULL;
    }

    TemplatorAVLNode* node = tree->root;
    int res;
    while ((res = tree->config->compareKeys(keyHint, tree->config->getKey(&node->dataAnchror))) != 0) {
        if ((res < 0 && node->left == NULL) || (res > 0 && node->right == NULL)) {
            return NULL;
        }
        if (res < 0) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return &node->dataAnchror;
}

#define EXTRACT_DEPTH(node, prefix)           \
    int prefix##left = 0;                     \
    int prefix##right = 0;                    \
    if ((node)->left != NULL) {               \
        prefix##left = (node)->left->depth;   \
    }                                         \
    if ((node)->right != NULL) {              \
        prefix##right = (node)->right->depth; \
    }

#define SET_NODE(tree, node, function)               \
    if ((node)->parrent != NULL) {                   \
        TemplatorAVLNode* parrent = (node)->parrent; \
        if (parrent->left == (node)) {               \
            parrent->left = function(node);          \
        } else {                                     \
            parrent->right = function(node);         \
        }                                            \
    } else {                                         \
        (tree)->root = function(node);               \
    }

void templator_avl_tree_update_depth(TemplatorAVLNode* node) {
    EXTRACT_DEPTH(node, );
    node->depth = ((left > right) ? (uint16_t)left : (uint16_t)right) + 1;
}

void templator_avl_tree_bubble_up(TemplatorAVLTree* tree, TemplatorAVLNode* node) {
    while (node != NULL) {
        EXTRACT_DEPTH(node, );
        if (abs(left - right) > 1) {
            if (left > right) { // left-* case
                EXTRACT_DEPTH(node->left, left_);
                if (left_left >= left_right) { // left-left case
                    SET_NODE(tree, node, templator_avl_tree_left_left_rotate);
                } else { // left-right case
                    SET_NODE(tree, node, templator_avl_tree_left_right_rotate);
                }
            } else { // right-* case
                EXTRACT_DEPTH(node->right, right_);
                if (right_left > right_right) { // right-left case
                    SET_NODE(tree, node, templator_avl_tree_right_left_rotate);
                } else { // right-right case
                    SET_NODE(tree, node, templator_avl_tree_right_right_rotate);
                }
            }
        } else {
            templator_avl_tree_update_depth(node);
        }
        node = node->parrent;
    }
}

TemplatorAVLNode* templator_avl_tree_rotate_left(TemplatorAVLNode* node) {
    // src: https://www.geeksforgeeks.org/insertion-in-an-avl-tree/?ref=lbp
    // x, y can't be NULL
    // a, b, c can be NULL
    //    x          y   |
    //   / \        / \  |
    //  a   y  =>  x   c |
    //     / \    / \    |
    //    b   c  a   b   |
    TemplatorAVLNode* parrent = node->parrent;
    TemplatorAVLNode* x_node = node;
    // TemplatorAVLNode* a = x->left; - unused
    TemplatorAVLNode* y_node = x_node->right;
    TemplatorAVLNode* b_node = y_node->left;
    // TemplatorAVLNode* c = y->right; - unused

    y_node->left = x_node;
    x_node->parrent = y_node;
    x_node->right = b_node;
    templator_avl_tree_update_depth(x_node);
    if (b_node != NULL) {
        b_node->parrent = x_node;
    }

    y_node->parrent = parrent;
    templator_avl_tree_update_depth(y_node);
    return y_node;
}

TemplatorAVLNode* templator_avl_tree_rotate_right(TemplatorAVLNode* node) {
    // src: https://www.geeksforgeeks.org/insertion-in-an-avl-tree/?ref=lbp
    // x, y can't be NULL
    // a, b, c can be NULL
    //     y        x     |
    //    / \      / \    |
    //   x   c => a   y   |
    //  / \          / \  |
    // a   b        b   c |
    TemplatorAVLNode* parrent = node->parrent;
    TemplatorAVLNode* y_node = node;
    TemplatorAVLNode* x_node = y_node->left;
    // TemplatorAVLNode* c = y->right;
    // TemplatorAVLNode* a = x->left;
    TemplatorAVLNode* b_node = x_node->right;

    x_node->right = y_node;
    y_node->parrent = x_node;
    y_node->left = b_node;
    templator_avl_tree_update_depth(y_node);
    if (b_node != NULL) {
        b_node->parrent = y_node;
    }

    x_node->parrent = parrent;
    templator_avl_tree_update_depth(x_node);
    return x_node;
}

TemplatorAVLNode* templator_avl_tree_right_right_rotate(TemplatorAVLNode* node) {
    // src: https://www.geeksforgeeks.org/insertion-in-an-avl-tree/?ref=lbp
    //    z                               y        |
    //  /  \                            /   \      |
    // T1   y     Left Rotate(z)       z      x    |
    //     /  \   - - - - - - - ->    / \    / \   |
    //    T2   x                     T1  T2 T3  T4 |
    //        / \                                  |
    //      T3  T4                                 |

    return templator_avl_tree_rotate_left(node);
}

TemplatorAVLNode* templator_avl_tree_right_left_rotate(TemplatorAVLNode* node) {
    // src: https://www.geeksforgeeks.org/insertion-in-an-avl-tree/?ref=lbp
    //    z                            z                            x        |
    //   / \                          / \                          /  \      |
    // T1   y   Right Rotate (y)    T1   x      Left Rotate(z)   z      y    |
    //     / \  - - - - - - - - ->     /  \   - - - - - - - ->  / \    / \   |
    //    x   T4                      T2   y                  T1  T2  T3  T4 |
    //   / \                              /  \                               |
    // T2   T3                           T3  T4                              |

    TemplatorAVLNode* z_node = node;
    TemplatorAVLNode* y_node = node->right;
    z_node->right = templator_avl_tree_rotate_right(y_node);
    return templator_avl_tree_rotate_left(z_node);
}

TemplatorAVLNode* templator_avl_tree_left_right_rotate(TemplatorAVLNode* node) {
    // src: https://www.geeksforgeeks.org/insertion-in-an-avl-tree/?ref=lbp
    //      z                               z                           x       |
    //     / \                            /   \                        /  \     |
    //    y   T4  Left Rotate (y)        x    T4  Right Rotate(z)    y      z   |
    //   / \      - - - - - - - - ->    /  \      - - - - - - - ->  / \    / \  |
    // T1   x                          y    T3                    T1  T2 T3  T4 |
    //     / \                        / \                                       |
    //   T2   T3                    T1   T2                                     |

    TemplatorAVLNode* z_node = node;
    TemplatorAVLNode* y_node = z_node->left;
    z_node->left = templator_avl_tree_rotate_left(y_node);
    return templator_avl_tree_rotate_right(z_node);
}

TemplatorAVLNode* templator_avl_tree_left_left_rotate(TemplatorAVLNode* node) {
    // src: https://www.geeksforgeeks.org/insertion-in-an-avl-tree/?ref=lbp
    //         z                                      y        |
    //        / \                                   /   \      |
    //       y   T4      Right Rotate (z)          x      z    |
    //      / \          - - - - - - - - ->      /  \    /  \  |
    //     x   T3                               T1  T2  T3  T4 |
    //    / \                                                  |
    //  T1   T2                                                |

    return templator_avl_tree_rotate_right(node);
}

void* templator_avl_tree_helper_identity(void* self) {
    return self;
}

void templator_avl_tree_helper_nop(void* /*item*/) { }
