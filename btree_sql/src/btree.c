#include "../include/btree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

BTree* btree_create(int order) {
    BTree *tree = (BTree*)malloc(sizeof(BTree));
    tree->root = btree_node_create(order, true);
    tree->order = order;
    return tree;
}

BTreeNode* btree_node_create(int order, bool is_leaf) {
    BTreeNode *node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->keys = (int64_t*)malloc((2 * order - 1) * sizeof(int64_t));
    node->children = (BTreeNode**)malloc(2 * order * sizeof(BTreeNode*));
    node->row_ids = (int64_t*)malloc((2 * order - 1) * sizeof(int64_t));
    node->num_keys = 0;
    node->is_leaf = is_leaf;

    for (int i = 0; i < 2 * order; i++) {
        node->children[i] = NULL;
    }

    return node;
}

void btree_split_child(BTreeNode *parent, int index, BTreeNode *child, int order) {
    BTreeNode *new_node = btree_node_create(order, child->is_leaf);
    new_node->num_keys = order - 1;

    // Copy the last (order-1) keys of child to new_node
    for (int i = 0; i < order - 1; i++) {
        new_node->keys[i] = child->keys[i + order];
        new_node->row_ids[i] = child->row_ids[i + order];
    }

    // Copy the last order children of child to new_node
    if (!child->is_leaf) {
        for (int i = 0; i < order; i++) {
            new_node->children[i] = child->children[i + order];
        }
    }

    child->num_keys = order - 1;

    // Shift parent's children to make room for new_node
    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
    }
    parent->children[index + 1] = new_node;

    // Move a key from child to parent
    for (int i = parent->num_keys - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
        parent->row_ids[i + 1] = parent->row_ids[i];
    }
    parent->keys[index] = child->keys[order - 1];
    parent->row_ids[index] = child->row_ids[order - 1];
    parent->num_keys++;
}

void btree_insert_non_full(BTreeNode *node, int64_t key, int64_t row_id, int order) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        // Find location and shift keys
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            node->row_ids[i + 1] = node->row_ids[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->row_ids[i + 1] = row_id;
        node->num_keys++;
    } else {
        // Find child to insert into
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;

        // Check if child is full
        if (node->children[i]->num_keys == 2 * order - 1) {
            btree_split_child(node, i, node->children[i], order);
            if (key > node->keys[i]) {
                i++;
            }
        }
        btree_insert_non_full(node->children[i], key, row_id, order);
    }
}

void btree_insert(BTree *tree, int64_t key, int64_t row_id) {
    BTreeNode *root = tree->root;

    // If root is full, split it
    if (root->num_keys == 2 * tree->order - 1) {
        BTreeNode *new_root = btree_node_create(tree->order, false);
        new_root->children[0] = root;
        btree_split_child(new_root, 0, root, tree->order);
        tree->root = new_root;
        btree_insert_non_full(new_root, key, row_id, tree->order);
    } else {
        btree_insert_non_full(root, key, row_id, tree->order);
    }
}

int64_t btree_search_node(BTreeNode *node, int64_t key) {
    int i = 0;
    while (i < node->num_keys && key > node->keys[i]) {
        i++;
    }

    if (i < node->num_keys && key == node->keys[i]) {
        return node->row_ids[i];
    }

    if (node->is_leaf) {
        return -1;  // Not found
    }

    return btree_search_node(node->children[i], key);
}

int64_t btree_search(BTree *tree, int64_t key) {
    if (tree->root == NULL) {
        return -1;
    }
    return btree_search_node(tree->root, key);
}

void btree_print_node(BTreeNode *node, int level) {
    if (node == NULL) return;

    printf("Level %d: ", level);
    for (int i = 0; i < node->num_keys; i++) {
        printf("%ld ", node->keys[i]);
    }
    printf("\n");

    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            btree_print_node(node->children[i], level + 1);
        }
    }
}

void btree_print(BTree *tree) {
    printf("B-Tree structure:\n");
    btree_print_node(tree->root, 0);
}

void btree_node_destroy(BTreeNode *node) {
    if (node == NULL) return;

    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            btree_node_destroy(node->children[i]);
        }
    }

    free(node->keys);
    free(node->children);
    free(node->row_ids);
    free(node);
}

void btree_destroy(BTree *tree) {
    if (tree == NULL) return;
    btree_node_destroy(tree->root);
    free(tree);
}

void btree_delete(BTree *tree, int64_t key) {
    // Simplified: not implementing delete for now
    printf("Delete operation not yet implemented\n");
}
