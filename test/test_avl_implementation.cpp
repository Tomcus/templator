#include <catch2/catch_test_macros.hpp>

#include "templator/avl_tree.h"

int cmpInt(int* a, int* b) {
    return *a - *b;
}

typedef struct {
    int key;
    int value;
} TestData;

void test_node(TemplatorAVLNode* node, int value, uint16_t depth) {
    REQUIRE(node != NULL);
    TestData* a = (TestData*)&node->dataAnchror;
    REQUIRE(depth == node->depth);
    REQUIRE(value == a->key);
    REQUIRE(value == a->value);
}

void insert_into_tree(TemplatorAVLTree* tree, int key) {
    TestData* d = (TestData*)templator_avl_tree_insert_with_key_hint(tree, &key);
    d->key = key;
    d->value = key;
}

TEST_CASE("Test basic implementation", "[avl]") {
    TemplatorAVLTreeConfig config;
    config.valueSize = sizeof(TestData);
    config.getKey = templator_avl_tree_helper_identity;
    config.compareKeys = (int(*)(void*, void*))cmpInt;
    config.freeNodeData = templator_avl_tree_helper_nop;

    TemplatorAVLTree tree;
    templator_avl_tree_init(&tree, &config);

    REQUIRE(tree.root == NULL);

    insert_into_tree(&tree, 10);
    test_node(tree.root, 10, 1);
    REQUIRE(tree.root->left == NULL);
    REQUIRE(tree.root->right == NULL);
    
    insert_into_tree(&tree, 9);
    test_node(tree.root, 10, 2);
    test_node(tree.root->left, 9, 1);
    REQUIRE(tree.root->left->left == NULL);
    REQUIRE(tree.root->left->right == NULL);
    REQUIRE(tree.root->right == NULL);
    
    insert_into_tree(&tree, 8);
    test_node(tree.root, 9, 2);
    test_node(tree.root->left, 8, 1);
    test_node(tree.root->right, 10, 1);
    REQUIRE(tree.root->left->left == NULL);
    REQUIRE(tree.root->left->right == NULL);
    REQUIRE(tree.root->right->left == NULL);
    REQUIRE(tree.root->right->right == NULL);
    
    insert_into_tree(&tree, 7);
    test_node(tree.root, 9, 3);
    test_node(tree.root->left, 8, 2);
    test_node(tree.root->left->left, 7, 1);
    test_node(tree.root->right, 10, 1);
    REQUIRE(tree.root->left->left->left == NULL);
    REQUIRE(tree.root->left->left->right == NULL);
    REQUIRE(tree.root->left->right == NULL);
    REQUIRE(tree.root->right->left == NULL);
    REQUIRE(tree.root->right->right == NULL);
    
    insert_into_tree(&tree, 6);
    test_node(tree.root, 9, 3);
    test_node(tree.root->left, 7, 2);
    test_node(tree.root->left->left, 6, 1);
    test_node(tree.root->left->right, 8, 1);
    test_node(tree.root->right, 10, 1);
    REQUIRE(tree.root->left->left->left == NULL);
    REQUIRE(tree.root->left->left->right == NULL);
    REQUIRE(tree.root->left->right->left == NULL);
    REQUIRE(tree.root->left->right->right == NULL);
    REQUIRE(tree.root->right->left == NULL);
    REQUIRE(tree.root->right->right == NULL);

    insert_into_tree(&tree, 5);
    test_node(tree.root, 7, 3);
    test_node(tree.root->left, 6, 2);
    test_node(tree.root->left->left, 5, 1);
    test_node(tree.root->right, 9, 2);
    test_node(tree.root->right->left, 8, 1);
    test_node(tree.root->right->right, 10, 1);
    REQUIRE(tree.root->left->left->left == NULL);
    REQUIRE(tree.root->left->left->right == NULL);
    REQUIRE(tree.root->left->right == NULL);
    REQUIRE(tree.root->right->left->left == NULL);
    REQUIRE(tree.root->right->left->right == NULL);
    REQUIRE(tree.root->right->right->left == NULL);
    REQUIRE(tree.root->right->right->right == NULL);

    insert_into_tree(&tree, 4);
    test_node(tree.root, 7, 3);
    test_node(tree.root->left, 5, 2);
    test_node(tree.root->left->left, 4, 1);
    test_node(tree.root->left->right, 6, 1);
    test_node(tree.root->right, 9, 2);
    test_node(tree.root->right->left, 8, 1);
    test_node(tree.root->right->right, 10, 1);
    REQUIRE(tree.root->left->left->left == NULL);
    REQUIRE(tree.root->left->left->right == NULL);
    REQUIRE(tree.root->left->right->left == NULL);
    REQUIRE(tree.root->left->right->right == NULL);
    REQUIRE(tree.root->right->left->left == NULL);
    REQUIRE(tree.root->right->left->right == NULL);
    REQUIRE(tree.root->right->right->left == NULL);
    REQUIRE(tree.root->right->right->right == NULL);

    templator_avl_tree_free(&tree);
}

TEST_CASE("Test left-right implementation", "[avl]") {
    TemplatorAVLTreeConfig config;
    config.valueSize = sizeof(TestData);
    config.getKey = templator_avl_tree_helper_identity;
    config.compareKeys = (int(*)(void*, void*))cmpInt;
    config.freeNodeData = templator_avl_tree_helper_nop;

    TemplatorAVLTree tree;
    templator_avl_tree_init(&tree, &config);

    insert_into_tree(&tree, 3);
    insert_into_tree(&tree, 1);
    insert_into_tree(&tree, 2);

    test_node(tree.root, 2, 2);
    test_node(tree.root->left, 1, 1);
    test_node(tree.root->right, 3, 1);

    templator_avl_tree_free(&tree);
}


TEST_CASE("Test right-left implementation", "[avl]") {
    TemplatorAVLTreeConfig config;
    config.valueSize = sizeof(TestData);
    config.getKey = templator_avl_tree_helper_identity;
    config.compareKeys = (int(*)(void*, void*))cmpInt;
    config.freeNodeData = templator_avl_tree_helper_nop;

    TemplatorAVLTree tree;
    templator_avl_tree_init(&tree, &config);

    insert_into_tree(&tree, 1);
    insert_into_tree(&tree, 3);
    insert_into_tree(&tree, 2);

    test_node(tree.root, 2, 2);
    test_node(tree.root->left, 1, 1);
    test_node(tree.root->right, 3, 1);

    templator_avl_tree_free(&tree);
}

TEST_CASE("Test right-right implementation", "[avl]") {
    TemplatorAVLTreeConfig config;
    config.valueSize = sizeof(TestData);
    config.getKey = templator_avl_tree_helper_identity;
    config.compareKeys = (int(*)(void*, void*))cmpInt;
    config.freeNodeData = templator_avl_tree_helper_nop;

    TemplatorAVLTree tree;
    templator_avl_tree_init(&tree, &config);

    insert_into_tree(&tree, 1);
    insert_into_tree(&tree, 2);
    insert_into_tree(&tree, 3);

    test_node(tree.root, 2, 2);
    test_node(tree.root->left, 1, 1);
    test_node(tree.root->right, 3, 1);

    templator_avl_tree_free(&tree);
}
