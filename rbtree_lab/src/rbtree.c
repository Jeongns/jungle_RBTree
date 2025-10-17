#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    p->nil = malloc(sizeof(node_t));
    p->nil->color = RBTREE_BLACK;
    p->root = p->nil;
    p->root->parent = p->nil;
    return p;
}

void delete_rbtree(rbtree *t) {
    delete_rbtree_node(t, t->root);
    free(t);
}

void delete_rbtree_node(rbtree *t, node_t *node) {
    if (node == t->nil) return;
    delete_rbtree_node(t, node->left);
    delete_rbtree_node(t, node->right);
    free(node);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
    // 노드 생성
    node_t *node = calloc(1, sizeof(node_t));
    node->color = RBTREE_RED;
    node->key = key;
    node->parent = t->nil;
    node->left = t->nil;
    node->right = t->nil;

    if (t->root == t->nil) {
        node->color = RBTREE_BLACK;
        t->root = node;
        return t->root;
    }

    node_t *parentNode;
    node_t *currentNode = t->root;
    while (currentNode != t->nil) {
        parentNode = currentNode;
        currentNode = currentNode->key > key ? currentNode->left : currentNode->right;
    }
    node->parent = parentNode;

    if (parentNode->key > key)
        parentNode->left = node;
    else
        parentNode->right = node;

    rbtreeInsertFixup(t, node);

    return t->root;
}

void rbtreeInsertFixup(rbtree *t, node_t *node) {
    node_t *uncle;
    while (node->parent->color == RBTREE_RED) {
        if (node->parent == node->parent->parent->left) {
            uncle = node->parent->parent->right;
            if (uncle->color == RBTREE_RED) {  // CASE 1
                node->parent->color = RBTREE_BLACK;
                uncle->color = RBTREE_BLACK;
                node->parent->parent->color = RBTREE_RED;
                node = node->parent->parent;
                continue;
            }
            if (node == node->parent->right) {  // CASE 2
                node = node->parent;
                leftRotate(t, node);
            }
            node->parent->color = RBTREE_BLACK;  // CASE 3
            node->parent->parent->color = RBTREE_RED;
            rightRotate(t, node->parent->parent);
        } else {
            uncle = node->parent->parent->left;
            if (uncle->color == RBTREE_RED) {  // CASE 1
                node->parent->color = RBTREE_BLACK;
                uncle->color = RBTREE_BLACK;
                node->parent->parent->color = RBTREE_RED;
                node = node->parent->parent;
                continue;
            }
            if (node == node->parent->left) {  // CASE 2
                node = node->parent;
                rightRotate(t, node);
            }
            node->parent->color = RBTREE_BLACK;  // CASE 3
            node->parent->parent->color = RBTREE_RED;
            leftRotate(t, node->parent->parent);
        }
    }
    t->root->color = RBTREE_BLACK;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
    node_t *currentNode = t->root;
    while (currentNode != t->nil) {
        if (currentNode->key == key) break;
        currentNode = currentNode->key > key ? currentNode->left : currentNode->right;
    }

    return currentNode == t->nil ? NULL : currentNode;
}

node_t *getMininumNode(const rbtree *t, const node_t *currentNode) {
    while (currentNode != t->nil) {
        if (currentNode->left == t->nil) break;
        currentNode = currentNode->left;
    }
    return currentNode;
}

node_t *rbtree_min(const rbtree *t) {
    return t->root == t->nil ? NULL : getMininumNode(t, t->root);
}

node_t *getMaximumNode(const rbtree *t, const node_t *currentNode) {
    while (currentNode != t->nil) {
        if (currentNode->right == t->nil) break;
        currentNode = currentNode->right;
    }
    return currentNode;
}

node_t *rbtree_max(const rbtree *t) {
    return t->root == t->nil ? NULL : getMaximumNode(t, t->root);
}

int rbtree_erase(rbtree *t, node_t *node) {
    node_t *fixNode;
    node_t *replaceNode = node;
    color_t deleteColor = node->color;

    if (!(node->left != t->nil && node->right != t->nil)) {
        fixNode = node->left == t->nil ? node->right : node->left;
        nodeTransplant(t, node, node->left == t->nil ? node->right : node->left);
    } else {
        replaceNode = getMininumNode(t, node->right);
        deleteColor = replaceNode->color;
        fixNode = replaceNode->right;
        if (replaceNode != node->right) {
            nodeTransplant(t, replaceNode, replaceNode->right);
            replaceNode->right = node->right;
            replaceNode->right->parent = replaceNode;
        } else {
            fixNode->parent = replaceNode;
        }
        nodeTransplant(t, node, replaceNode);
        replaceNode->left = node->left;
        replaceNode->left->parent = replaceNode;
        replaceNode->color = node->color;
    }

    if (deleteColor == RBTREE_BLACK) rbtreeDeleteFixup(t, fixNode);
    free(node);
    return 0;
}

void rbtreeDeleteFixup(rbtree *t, node_t *fixNode) {
    node_t *brotherNode;
    while (fixNode != t->root && fixNode->color == RBTREE_BLACK) {
        if (fixNode == fixNode->parent->left) {
            node_t *brotherNode = fixNode->parent->right;
            // CASE 1
            if (brotherNode->color == RBTREE_RED) {
                brotherNode->color = RBTREE_BLACK;
                fixNode->parent->color = RBTREE_RED;
                leftRotate(t, fixNode->parent);
                brotherNode = fixNode->parent->right;
            }
            // CASE 2
            if (brotherNode->left->color == RBTREE_BLACK && brotherNode->right->color) {
                brotherNode->color = RBTREE_RED;
                fixNode = fixNode->parent;
            } else {
                // CASE 3
                if (brotherNode->right->color == RBTREE_BLACK) {
                    brotherNode->left->color = RBTREE_BLACK;
                    brotherNode->color = RBTREE_RED;
                    rightRotate(t, brotherNode);
                    brotherNode = fixNode->parent->right;
                }
                // CASE 4
                brotherNode->color = brotherNode->parent->color;
                fixNode->parent->color = RBTREE_BLACK;
                brotherNode->right->color = RBTREE_BLACK;
                leftRotate(t, fixNode->parent);
                fixNode = t->root;
            }
        } else {
            node_t *brotherNode = fixNode->parent->left;
            // CASE 1
            if (brotherNode->color == RBTREE_RED) {
                brotherNode->color = RBTREE_BLACK;
                fixNode->parent->color = RBTREE_RED;
                rightRotate(t, fixNode->parent);
                brotherNode = fixNode->parent->left;
            }
            // CASE 2
            if (brotherNode->left->color == RBTREE_BLACK && brotherNode->right->color) {
                brotherNode->color = RBTREE_RED;
                fixNode = fixNode->parent;
            } else {
                // CASE 3
                if (brotherNode->left->color == RBTREE_BLACK) {
                    brotherNode->right->color = RBTREE_BLACK;
                    brotherNode->color = RBTREE_RED;
                    leftRotate(t, brotherNode);
                    brotherNode = fixNode->parent->left;
                }
                // CASE 4
                brotherNode->color = brotherNode->parent->color;
                fixNode->parent->color = RBTREE_BLACK;
                brotherNode->left->color = RBTREE_BLACK;
                rightRotate(t, fixNode->parent);
                fixNode = t->root;
            }
        }
    }
    fixNode->color = RBTREE_BLACK;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    int index = 0;
    inorderToArray(t, t->root, arr, &index);
    return 0;
}

void inorderToArray(const rbtree *t, node_t *node, key_t *arr, int *index) {
    if (node == t->nil) return;
    inorderToArray(t, node->left, arr, index);
    arr[(*index)++] = node->key;
    inorderToArray(t, node->right, arr, index);
}

void rightRotate(rbtree *t, node_t *x) {
    node_t *y = x->left;

    x->left = y->right;
    if (y->right != t->nil) y->right->parent = x;

    y->parent = x->parent;
    if (x->parent == t->nil)
        t->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->right = x;
    x->parent = y;
}

void leftRotate(rbtree *t, node_t *x) {
    node_t *y = x->right;

    x->right = y->left;
    if (y->left != t->nil) y->left->parent = x;

    y->parent = x->parent;
    if (x->parent == t->nil)
        t->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void nodeTransplant(rbtree *t, node_t *u, node_t *v) {
    if (u->parent == t->nil)
        t->root = v;
    else if (u->parent->left == u)
        u->parent->left = v;
    else
        u->parent->right = v;

    v->parent = u->parent;
}