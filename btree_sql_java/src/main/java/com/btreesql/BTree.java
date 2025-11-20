package com.btreesql;

import java.util.ArrayList;
import java.util.List;

/**
 * B-Tree implementation for indexing
 */
public class BTree {
    private static final int ORDER = 4; // Minimum degree

    private BTreeNode root;

    public BTree() {
        this.root = new BTreeNode(true);
    }

    public void insert(long key, long rowId) {
        BTreeNode r = root;

        if (r.isFull()) {
            BTreeNode newRoot = new BTreeNode(false);
            newRoot.children.add(r);
            splitChild(newRoot, 0);
            root = newRoot;
            insertNonFull(newRoot, key, rowId);
        } else {
            insertNonFull(r, key, rowId);
        }
    }

    private void insertNonFull(BTreeNode node, long key, long rowId) {
        int i = node.keys.size() - 1;

        if (node.isLeaf) {
            // Insert key in sorted order
            node.keys.add(0L);
            node.rowIds.add(0L);

            while (i >= 0 && key < node.keys.get(i)) {
                node.keys.set(i + 1, node.keys.get(i));
                node.rowIds.set(i + 1, node.rowIds.get(i));
                i--;
            }

            node.keys.set(i + 1, key);
            node.rowIds.set(i + 1, rowId);
        } else {
            // Find child to insert into
            while (i >= 0 && key < node.keys.get(i)) {
                i--;
            }
            i++;

            if (node.children.get(i).isFull()) {
                splitChild(node, i);
                if (key > node.keys.get(i)) {
                    i++;
                }
            }

            insertNonFull(node.children.get(i), key, rowId);
        }
    }

    private void splitChild(BTreeNode parent, int index) {
        BTreeNode child = parent.children.get(index);
        BTreeNode newNode = new BTreeNode(child.isLeaf);

        int midPoint = ORDER - 1;

        // Copy second half of keys to new node
        for (int i = midPoint + 1; i < child.keys.size(); i++) {
            newNode.keys.add(child.keys.get(i));
            newNode.rowIds.add(child.rowIds.get(i));
        }

        // If not a leaf, copy children too
        if (!child.isLeaf) {
            for (int i = midPoint + 1; i < child.children.size(); i++) {
                newNode.children.add(child.children.get(i));
            }
        }

        // Move middle key up to parent
        long middleKey = child.keys.get(midPoint);
        long middleRowId = child.rowIds.get(midPoint);

        // Trim child node
        if (!child.isLeaf) {
            child.children.subList(midPoint + 1, child.children.size()).clear();
        }
        child.keys.subList(midPoint, child.keys.size()).clear();
        child.rowIds.subList(midPoint, child.rowIds.size()).clear();

        // Insert into parent
        parent.keys.add(index, middleKey);
        parent.rowIds.add(index, middleRowId);
        parent.children.add(index + 1, newNode);
    }

    public Long search(long key) {
        return search(root, key);
    }

    private Long search(BTreeNode node, long key) {
        int i = 0;
        while (i < node.keys.size() && key > node.keys.get(i)) {
            i++;
        }

        if (i < node.keys.size() && key == node.keys.get(i)) {
            return node.rowIds.get(i);
        }

        if (node.isLeaf) {
            return null; // Not found
        }

        return search(node.children.get(i), key);
    }

    public void print() {
        System.out.println("B-Tree structure:");
        print(root, 0);
    }

    private void print(BTreeNode node, int level) {
        System.out.print("Level " + level + ": ");
        for (long key : node.keys) {
            System.out.print(key + " ");
        }
        System.out.println();

        if (!node.isLeaf) {
            for (BTreeNode child : node.children) {
                print(child, level + 1);
            }
        }
    }

    /**
     * B-Tree Node class
     */
    private static class BTreeNode {
        List<Long> keys;
        List<Long> rowIds;
        List<BTreeNode> children;
        boolean isLeaf;

        BTreeNode(boolean isLeaf) {
            this.keys = new ArrayList<>();
            this.rowIds = new ArrayList<>();
            this.children = new ArrayList<>();
            this.isLeaf = isLeaf;
        }

        boolean isFull() {
            return keys.size() >= 2 * ORDER - 1;
        }
    }
}
