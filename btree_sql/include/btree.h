#ifndef BTREE_H
#define BTREE_H

#include <stdint.h>
#include <stdbool.h>

#define BTREE_ORDER 4  // Minimum degree (minimum number of keys = ORDER-1)

typedef struct BTreeNode {
    int64_t *keys;              // Array of keys
    struct BTreeNode **children; // Array of child pointers
    int num_keys;               // Current number of keys
    bool is_leaf;               // Is true when node is leaf
    int64_t *row_ids;           // Array of row IDs (for leaf nodes)
} BTreeNode;

typedef struct BTree {
    BTreeNode *root;
    int order;
} BTree;

// B-tree operations
BTree* btree_create(int order);
BTreeNode* btree_node_create(int order, bool is_leaf);
void btree_insert(BTree *tree, int64_t key, int64_t row_id);
int64_t btree_search(BTree *tree, int64_t key);
void btree_delete(BTree *tree, int64_t key);
void btree_destroy(BTree *tree);
void btree_print(BTree *tree);

// Helper functions
void btree_split_child(BTreeNode *parent, int index, BTreeNode *child, int order);
void btree_insert_non_full(BTreeNode *node, int64_t key, int64_t row_id, int order);

#endif // BTREE_H
