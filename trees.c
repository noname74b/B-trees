#include <stdlib.h>
#include <stdio.h>
#include "trees.h"

// Структура узла 2-3-дерева
typedef struct Node {
    int keys[2];              // Может хранить 1 или 2 ключа
    struct Node* children[3]; // Может иметь 2 или 3 потомка
    int n;                    // Количество ключей (1 или 2)
    int is_leaf;            // Является ли листом
} Node;

// Создание нового узла
Node* create_node(int is_leaf) {
    Node* node = malloc(sizeof(Node*));
    node->n = 0;
    node->is_leaf = is_leaf;
    for (int i = 0; i < 3; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// Поиск ключа в дереве (рекурсивный)
Node* search(Node* root, int key) {
    if (root == NULL) return NULL;
    
    for (int i = 0; i < root->n; i++) {
        if (key == root->keys[i]) {
            return root;
        }
    }

    if (root->is_leaf) return NULL;
    
    if (key < root->keys[0]) {
        return search(root->children[0], key);
    } else if (root->n == 1 || key < root->keys[1]) {
        return search(root->children[1], key);
    } else {
        return search(root->children[2], key);
    }
}

// Вставка ключа в листовой узел (простая вставка с сортировкой)
void insert_in_leaf(Node* leaf, int key) {
    if (leaf->n == 0) {
        leaf->keys[0] = key;
        leaf->n = 1;
    } else if (leaf->n == 1) {
        if (key < leaf->keys[0]) {
            leaf->keys[1] = leaf->keys[0];
            leaf->keys[0] = key;
        } else {
            leaf->keys[1] = key;
        }
        leaf->n = 2;
    }
}

// Рекурсивная вставка
Node* insert_in_root(Node* root, int key) {

    if (root->is_leaf) {
        // Если лист не полный - просто вставляем
        if (root->n < 2) {
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
        root->n = 1;
        
        Node* sibling = create_node(1);
        sibling->keys[0] = temp_keys[2];
        sibling->n = 1;
        
        Node* new_root = create_node(1);
        new_root->keys[0] = temp_keys[1];
        new_root->n = 1;
        new_root->children[0] = root;
        new_root->children[1] = sibling;
        
        return new_root;
    }

    Node* result = NULL;
    
    if (key < root->keys[0]) {
        result = insert_in_root(root->children[0], key);
    } else if (root->n == 1 || key < root->keys[1]) {
        result = insert_in_root(root->children[1], key);
    } else {
        result = insert_in_root(root->children[2], key);
    }
    
    return result ? result : root;
}

// Основная функция вставки
Node* insert(Node* root, int key) {
    // Проверяем, нет ли уже такого ключа
    if (search(root, key) != NULL) {
        printf("Ключ %d уже существует!\n", key);
        return root;
    }
    
    // Если дерево пустое
    if (root == NULL) {
        root = create_node(1);
        root->keys[0] = key;
        root->n = 1;
        return root;
    }
    
    return insert_in_root(root, key);
}

// Обход дерева (in-order)
void traverse(Node* root) {
    if (root == NULL) return;
    
    if (root->is_leaf) {
        for (int i = 0; i < root->n; i++) {
            printf("%d ", root->keys[i]);
        }
    } else {
        if (root->n == 1) {
            // 2-узел
            traverse(root->children[0]);
            printf("%d ", root->keys[0]);
            traverse(root->children[1]);
        } else {
            // 3-узел
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
    if (root->n == 1) {
        printf("2-узел): [%d]\n", root->keys[0]);
    } else {
        printf("3-узел): [%d, %d]\n", root->keys[0], root->keys[1]);
    }
    
    if (!root->is_leaf) {
        for (int i = 0; i <= root->n; i++) {
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
        for (int i = 0; i <= root->n; i++) {
            free_tree(root->children[i]);
        }
    }
    free(root);
}

