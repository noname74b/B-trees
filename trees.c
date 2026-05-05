#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "trees.h"



// Создание нового узла
Node* create_node() {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key_count = 0;
    for (int i = 0; i < 3; i++) {
        node->children[i] = NULL;
    }
    node->parent = NULL;
    return node;
}

// Проверка, является ли узел листом
bool is_leaf(Node* node) {
    return node->children[0] == NULL;
}

// Вставка ключа в узел (предполагается, что есть место)
void insert_key_into_node(Node* node, int key, Node* left_child, Node* right_child) {
    int pos = 0;
    while (pos < node->key_count && key > node->keys[pos]) {
        pos++;
    }
    
    for (int i = node->key_count; i > pos; i--) {
        node->keys[i] = node->keys[i - 1];
    }
    
    node->keys[pos] = key;
    node->key_count++;
    
    // Вставляем дочерние узлы, если они есть
    if (left_child != NULL && right_child != NULL) {
        for (int i = node->key_count; i > pos + 1; i--) {
            node->children[i] = node->children[i - 1];
        }
        node->children[pos] = left_child;
        node->children[pos + 1] = right_child;
        
        left_child->parent = node;
        right_child->parent = node;
    }
}

// Вставка с возвратом необходимости разделения
bool insert_helper(Node* node, int key, int* promote_key, Node** promote_node) {
    if (is_leaf(node)) {
        if (node->key_count < 2) {
            insert_key_into_node(node, key, NULL, NULL);
            return false;
        } else {
            int temp_keys[3];
            for (int i = 0; i < 2; i++) {
                temp_keys[i] = node->keys[i];
            }
            temp_keys[2] = key;
            
            for (int i = 0; i < 2; i++) {
                for (int j = i + 1; j < 3; j++) {
                    if (temp_keys[i] > temp_keys[j]) {
                        int temp = temp_keys[i];
                        temp_keys[i] = temp_keys[j];
                        temp_keys[j] = temp;
                    }
                }
            }
            
            *promote_key = temp_keys[1];
            
            *promote_node = create_node();
            (*promote_node)->key_count = 1;
            (*promote_node)->keys[0] = temp_keys[2];
            
            node->key_count = 1;
            node->keys[0] = temp_keys[0];
            
            return true;
        }
    } else {
        int child_index = 0;
        while (child_index < node->key_count && key > node->keys[child_index]) {
            child_index++;
        }
        
        Node* child = node->children[child_index];
        int child_promote_key;
        Node* child_promote_node;
        
        bool need_split = insert_helper(child, key, &child_promote_key, &child_promote_node);
        
        if (!need_split) {
            return false;
        }
        
        if (node->key_count < 2) {
            insert_key_into_node(node, child_promote_key, child, child_promote_node);
            return false;
        } else {
            Node* temp_children[4];
            int temp_keys[3];
            
            for (int i = 0; i < 2; i++) {
                temp_keys[i] = node->keys[i];
            }
            
            for (int i = 0; i <= 2; i++) {
                temp_children[i] = node->children[i];
            }
            
            int pos = 0;
            while (pos < 2 && child_promote_key > temp_keys[pos]) {
                pos++;
            }
            
            for (int i = 2; i > pos; i--) {
                temp_keys[i] = temp_keys[i - 1];
                temp_children[i + 1] = temp_children[i];
            }
            temp_keys[pos] = child_promote_key;
            temp_children[pos + 1] = child_promote_node;
            temp_children[pos] = child;
            
            *promote_key = temp_keys[1];
            
            *promote_node = create_node();
            
            node->key_count = 1;
            node->keys[0] = temp_keys[0];
            node->children[0] = temp_children[0];
            node->children[1] = temp_children[1];
            
            (*promote_node)->key_count = 1;
            (*promote_node)->keys[0] = temp_keys[2];
            (*promote_node)->children[0] = temp_children[2];
            (*promote_node)->children[1] = temp_children[3];
            
            node->children[2] = NULL;
            for (int i = 0; i <= 1; i++) {
                if (node->children[i]) node->children[i]->parent = node;
                if ((*promote_node)->children[i]) (*promote_node)->children[i]->parent = *promote_node;
            }
            
            (*promote_node)->parent = node->parent;
            
            return true;
        }
    }
}

// Основная функция вставки
Node* insert(Node* root, int key) {
    if (root == NULL) {
        root = create_node();
        root->key_count = 1;
        root->keys[0] = key;
        return root;
    }
    
    int promote_key;
    Node* promote_node;
    
    bool need_split = insert_helper(root, key, &promote_key, &promote_node);
    
    if (need_split) {
        // Создаем новый корень
        Node* new_root = create_node();
        new_root->key_count = 1;
        new_root->keys[0] = promote_key;
        new_root->children[0] = root;
        new_root->children[1] = promote_node;
        root->parent = new_root;
        promote_node->parent = new_root;
        return new_root;
    }
    
    return root;
}

// Поиск ключа
bool search(Node* root, int key) {
    if (root == NULL) return false;
    
    int i = 0;
    while (i < root->key_count && key > root->keys[i]) {
        i++;
    }
    
    if (i < root->key_count && key == root->keys[i]) {
        return true;
    }
    
    if (is_leaf(root)) {
        return false;
    }
    
    return search(root->children[i], key);
}

// Получение минимального ключа в поддереве
int get_min(Node* root) {
    if (root == NULL) return -1;
    
    if (is_leaf(root)) {
        return root->keys[0];
    }
    
    return get_min(root->children[0]);
}

// Удаление ключа (упрощенная версия)
Node* delete_key(Node* root, int key) {
    printf("Функция удаления требует полной реализации\n");
    return root;
}

// Вывод дерева (для отладки и/или просмотра)
void print_tree(Node* root, int level) {
    if (root == NULL) return;
    
    for (int i = 0; i < level-1; i++) {
        printf("  ");
    }
    if (level) {
        printf("->");
    }

    printf("[");
    for (int i = 0; i < root->key_count; i++) {
        printf("%d", root->keys[i]);
        if (i < root->key_count - 1) printf(", ");
    }
    printf("]\n");
    
    if (!is_leaf(root)) {
        for (int i = 0; i <= root->key_count; i++) {
            print_tree(root->children[i], level + 1);
        }
    }
}

// Обход дерева (по порядку)
void traverse(Node* root) {
    if (root == NULL) return;
    
    if (is_leaf(root)) {
        for (int i = 0; i < root->key_count; i++) {
            printf("%d ", root->keys[i]);
        }
    } else {
        if (root->key_count == 1) {
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

// Освобождение памяти
void free_tree(Node* root) {
    if (root == NULL) return;
    
    if (!is_leaf(root)) {
        for (int i = 0; i <= root->key_count; i++) {
            free_tree(root->children[i]);
        }
    }
    free(root);
}
