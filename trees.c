#include <stdlib.h>
#include <stdio.h>
#include "trees.h"

typedef struct Node {
    int keys[2];     
    struct Node* children[3];
    struct Node* parent;
    int size;                    // Количество ключей (1 или 2)
    int is_leaf;          
}Node;

// Создание нового узла
Node* create_node(int is_leaf) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->size = 0;
    node->is_leaf = is_leaf;
    for (int i = 0; i < 3; i++) {
        node->children[i] = NULL;
    }
    node->parent = NULL;
    return node;
}

// Поиск ключа в дереве (рекурсивный)
Node* search(Node* root, int key) {
    if (root == NULL) return NULL;
    
    for (int i = 0; i < root->size; i++) {
        if (key == root->keys[i]) {
            return root;
        }
    }

    if (root->is_leaf) return NULL;
    
    if (key < root->keys[0]) {
        return search(root->children[0], key);
    } else if (root->size == 1 || key < root->keys[1]) {
        return search(root->children[1], key);
    } else {
        return search(root->children[2], key);
    }
}

// простая вставка с сортировкой
void insert_in_leaf(Node* leaf, int key) {
    if (leaf->size == 0) {
        leaf->keys[0] = key;
        leaf->size = 1;
    } else if (leaf->size == 1) {
        if (key < leaf->keys[0]) {
            leaf->keys[1] = leaf->keys[0];
            leaf->keys[0] = key;
        } else {
            leaf->keys[1] = key;
        }
        leaf->size = 2;
    }
}

// Рекурсивная вставка
Node* insert_in_root(Node* root, int key) {

    if (root->is_leaf) {
        if (root->size < 2) {
            insert_in_leaf(root, key);
            return root;
        }

        int temp_keys[3] = {root->keys[0], root->keys[1], key};
        
        // Сортируем три ключа
        for (int i = 0; i < 3; i++) {
            for (int j = i + 1; j < 3; j++) {
                if (temp_keys[i] > temp_keys[j]) {
                    int t = temp_keys[i];
                    temp_keys[i] = temp_keys[j];
                    temp_keys[j] = t;
                }
            }
        }
        
        root->keys[0] = temp_keys[0];
        root->size = 1;
        
        Node* child = create_node(1);
        child->keys[0] = temp_keys[2];
        child->size = 1;
        
        Node* new_root = create_node(0);
        new_root->keys[0] = temp_keys[1];
        new_root->size = 1;
        new_root->children[0] = root;
        new_root->children[1] = child; 
        root->parent = new_root;
        child->parent = new_root;

        return new_root;
    }
    
    if (key < root->keys[0]) {
        root->children[0] = insert_in_root(root->children[0], key);
    } else if (root->size == 1 || key < root->keys[1]) {
        root->children[1] = insert_in_root(root->children[1], key);
    } else {
        root->children[2] = insert_in_root(root->children[2], key);
    }

    return root;
}

// Основная функция вставки
Node* insert(Node* tree, int key) {
    if (search(tree, key) != NULL) {
        printf("Ключ %d уже существует!\n", key);
        return tree;
    }
    
    if (tree == NULL) {
        tree = create_node(1);
        tree->keys[0] = key;
        tree->size = 1;
        return tree;
    }
    
    return insert_in_root(tree, key);
}

// Обход дерева (in-order)
void traverse(Node* root) {
    if (root == NULL) {
        printf("empty tree\n");
        return;
    }
    
    if (root->is_leaf) {
        for (int i = 0; i < root->size; i++) {
            printf("%d ", root->keys[i]);
        }
    } else {
        if (root->size == 1) {
            traverse(root->children[0]);
            printf("%d ", root->keys[0]);
            traverse(root->children[1]);
        } else {
            traverse(root->children[0]);
            printf("%d ", root->keys[0]);
            traverse(root->children[1]);
            printf("%d ", root->keys[1]);
            traverse(root->children[2]);
        }
    }
}

// Вывод структуры дерева
void print_tree(Node* root, int level) {
    if (root == NULL) return;
    
    printf("Уровень %d (", level);
    if (root->size == 1) {
        printf("2-узел): [%d]\n", root->keys[0]);
    } else {
        printf("3-узел): [%d, %d]\n", root->keys[0], root->keys[1]);
    }
    
    if (!root->is_leaf) {
        for (int i = 0; i <= root->size; i++) {
            if (root->children[i] != NULL) {
                printf("Потомок %d: ", i);
                print_tree(root->children[i], level + 1);
            }
        }
    }
}

// Освобождение памяти
void free_tree(Node* root) {
    if (root == NULL) return;
    
    if (!root->is_leaf) {
        for (int i = 0; i <= root->size; i++) {
            free_tree(root->children[i]);
        }
    }
    free(root);
}

