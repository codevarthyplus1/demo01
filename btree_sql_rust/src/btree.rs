use std::fmt;

const BTREE_ORDER: usize = 4;

#[derive(Debug, Clone)]
pub struct BTreeNode {
    keys: Vec<i64>,
    row_ids: Vec<i64>,
    children: Vec<Box<BTreeNode>>,
    is_leaf: bool,
}

impl BTreeNode {
    fn new(is_leaf: bool) -> Self {
        BTreeNode {
            keys: Vec::with_capacity(2 * BTREE_ORDER - 1),
            row_ids: Vec::with_capacity(2 * BTREE_ORDER - 1),
            children: Vec::with_capacity(2 * BTREE_ORDER),
            is_leaf,
        }
    }

    fn is_full(&self) -> bool {
        self.keys.len() >= 2 * BTREE_ORDER - 1
    }

    fn search(&self, key: i64) -> Option<i64> {
        let mut i = 0;
        while i < self.keys.len() && key > self.keys[i] {
            i += 1;
        }

        if i < self.keys.len() && key == self.keys[i] {
            return Some(self.row_ids[i]);
        }

        if self.is_leaf {
            return None;
        }

        self.children[i].search(key)
    }

    fn insert_non_full(&mut self, key: i64, row_id: i64) {
        let mut i = self.keys.len();

        if self.is_leaf {
            // Insert key in sorted order
            self.keys.push(0);
            self.row_ids.push(0);

            while i > 0 && key < self.keys[i - 1] {
                self.keys[i] = self.keys[i - 1];
                self.row_ids[i] = self.row_ids[i - 1];
                i -= 1;
            }

            self.keys[i] = key;
            self.row_ids[i] = row_id;
        } else {
            // Find child to insert into
            while i > 0 && key < self.keys[i - 1] {
                i -= 1;
            }

            if self.children[i].is_full() {
                self.split_child(i);
                if key > self.keys[i] {
                    i += 1;
                }
            }

            self.children[i].insert_non_full(key, row_id);
        }
    }

    fn split_child(&mut self, index: usize) {
        let mut new_node = BTreeNode::new(self.children[index].is_leaf);
        let child = &mut self.children[index];

        // Copy second half of keys to new node
        new_node.keys = child.keys.split_off(BTREE_ORDER - 1);
        new_node.row_ids = child.row_ids.split_off(BTREE_ORDER - 1);

        // Move middle key up to parent
        let middle_key = new_node.keys.remove(0);
        let middle_row_id = new_node.row_ids.remove(0);

        // If not a leaf, split children too
        if !child.is_leaf {
            new_node.children = child.children.split_off(BTREE_ORDER);
        }

        // Insert middle key into parent
        self.keys.insert(index, middle_key);
        self.row_ids.insert(index, middle_row_id);
        self.children.insert(index + 1, Box::new(new_node));
    }

    fn print(&self, level: usize) {
        print!("Level {}: ", level);
        for key in &self.keys {
            print!("{} ", key);
        }
        println!();

        if !self.is_leaf {
            for child in &self.children {
                child.print(level + 1);
            }
        }
    }
}

pub struct BTree {
    root: Box<BTreeNode>,
}

impl BTree {
    pub fn new() -> Self {
        BTree {
            root: Box::new(BTreeNode::new(true)),
        }
    }

    pub fn insert(&mut self, key: i64, row_id: i64) {
        if self.root.is_full() {
            let mut new_root = BTreeNode::new(false);
            let old_root = std::mem::replace(&mut *self.root, BTreeNode::new(true));
            new_root.children.push(Box::new(old_root));
            new_root.split_child(0);
            new_root.insert_non_full(key, row_id);
            self.root = Box::new(new_root);
        } else {
            self.root.insert_non_full(key, row_id);
        }
    }

    pub fn search(&self, key: i64) -> Option<i64> {
        self.root.search(key)
    }

    pub fn print(&self) {
        println!("B-Tree structure:");
        self.root.print(0);
    }
}

impl fmt::Display for BTree {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "BTree with {} keys", self.root.keys.len())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_insert_and_search() {
        let mut tree = BTree::new();
        tree.insert(10, 1);
        tree.insert(20, 2);
        tree.insert(30, 3);

        assert_eq!(tree.search(10), Some(1));
        assert_eq!(tree.search(20), Some(2));
        assert_eq!(tree.search(30), Some(3));
        assert_eq!(tree.search(40), None);
    }

    #[test]
    fn test_split() {
        let mut tree = BTree::new();
        for i in 1..=10 {
            tree.insert(i, i);
        }

        for i in 1..=10 {
            assert_eq!(tree.search(i), Some(i));
        }
    }
}
