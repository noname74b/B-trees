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
    // Находим позицию для вставки
    int pos = 0;
    while (pos < node->key_count && key > node->keys[pos]) {
        pos++;
    }
    
    // Сдвигаем существующие ключи вправо
    for (int i = node->key_count; i > pos; i--) {
        node->keys[i] = node->keys[i - 1];
    }
    
    // Вставляем новый ключ
    node->keys[pos] = key;
    node->key_count++;
    
    // Вставляем дочерние узлы, если они есть
    if (left_child != NULL && right_child != NULL) {
        // Сдвигаем дочерние узлы
        for (int i = node->key_count; i > pos + 1; i--) {
            node->children[i] = node->children[i - 1];
        }
        node->children[pos] = left_child;
        node->children[pos + 1] = right_child;
        
        // Обновляем родителей
        left_child->parent = node;
        right_child->parent = node;
    }
}

// Разделение узла
void split_node(Node* node, int* middle_key, Node** new_node) {
    *new_node = create_node();
    
    // В 2-3 дереве разделение происходит только при 3 ключах
    // Временный массив для сортировки
    int temp_keys[3];
    Node* temp_children[4];
    
    // Копируем ключи во временный массив
    for (int i = 0; i < node->key_count; i++) {
        temp_keys[i] = node->keys[i];
    }
    
    // Добавляем новый ключ (уже должен быть вставлен перед вызовом split)
    // Сортируем ключи (пузырьковая сортировка для 3 элементов)
    for (int i = 0; i < 2; i++) {
        for (int j = i + 1; j < 3; j++) {
            if (temp_keys[i] > temp_keys[j]) {
                int temp = temp_keys[i];
                temp_keys[i] = temp_keys[j];
                temp_keys[j] = temp;
            }
        }
    }
    
    // Для внутренних узлов также копируем детей
    if (!is_leaf(node)) {
        for (int i = 0; i <= node->key_count; i++) {
            temp_children[i] = node->children[i];
        }
    }
    
    // Средний ключ поднимается вверх
    *middle_key = temp_keys[1];
    
    // Левый узел получает первый ключ
    node->key_count = 1;
    node->keys[0] = temp_keys[0];
    
    // Правый узел получает третий ключ
    (*new_node)->key_count = 1;
    (*new_node)->keys[0] = temp_keys[2];
    
    // Перераспределяем дочерние узлы
    if (!is_leaf(node)) {
        // Левому узлу
        node->children[0] = temp_children[0];
        node->children[1] = temp_children[1];
        
        // Правому узлу
        (*new_node)->children[0] = temp_children[2];
        (*new_node)->children[1] = temp_children[3];
        
        // Обновляем родителей
        if (temp_children[0]) temp_children[0]->parent = node;
        if (temp_children[1]) temp_children[1]->parent = node;
        if (temp_children[2]) temp_children[2]->parent = *new_node;
        if (temp_children[3]) temp_children[3]->parent = *new_node;
    }
    
    // Обнуляем остальные поля
    if (!is_leaf(node)) {
        node->children[2] = NULL;
    }
    (*new_node)->parent = node->parent;
}

// Вставка с возвратом необходимости разделения
bool insert_helper(Node* node, int key, int* promote_key, Node** promote_node) {
    if (is_leaf(node)) {
        // Вставляем ключ в лист
        if (node->key_count < 2) {
            // Место есть
            insert_key_into_node(node, key, NULL, NULL);
            return false;
        } else {
            // Нужно разделение
            // Временно вставляем третий ключ
            int temp_keys[3];
            for (int i = 0; i < 2; i++) {
                temp_keys[i] = node->keys[i];
            }
            temp_keys[2] = key;
            
            // Сортируем
            for (int i = 0; i < 2; i++) {
                for (int j = i + 1; j < 3; j++) {
                    if (temp_keys[i] > temp_keys[j]) {
                        int temp = temp_keys[i];
                        temp_keys[i] = temp_keys[j];
                        temp_keys[j] = temp;
                    }
                }
            }
            
            // Средний ключ для продвижения
            *promote_key = temp_keys[1];
            
            // Создаем новый узел
            *promote_node = create_node();
            (*promote_node)->key_count = 1;
            (*promote_node)->keys[0] = temp_keys[2];
            
            // Обновляем текущий узел
            node->key_count = 1;
            node->keys[0] = temp_keys[0];
            
            return true;
        }
    } else {
        // Внутренний узел - находим подходящего ребенка
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
        
        // Ребенок разделился, вставляем продвинутый ключ в текущий узел
        if (node->key_count < 2) {
            // В текущем узле есть место
            insert_key_into_node(node, child_promote_key, child, child_promote_node);
            return false;
        } else {
            // Текущий узел тоже нужно разделить
            // Сначала вставляем временно
            Node* temp_children[4];
            int temp_keys[3];
            
            // Копируем существующие ключи
            for (int i = 0; i < 2; i++) {
                temp_keys[i] = node->keys[i];
            }
            
            // Копируем существующих детей
            for (int i = 0; i <= 2; i++) {
                temp_children[i] = node->children[i];
            }
            
            // Вставляем новый ключ в правильную позицию
            int pos = 0;
            while (pos < 2 && child_promote_key > temp_keys[pos]) {
                pos++;
            }
            
            // Сдвигаем
            for (int i = 2; i > pos; i--) {
                temp_keys[i] = temp_keys[i - 1];
                temp_children[i + 1] = temp_children[i];
            }
            temp_keys[pos] = child_promote_key;
            temp_children[pos + 1] = child_promote_node;
            temp_children[pos] = child;
            
            // Средний ключ для продвижения
            *promote_key = temp_keys[1];
            
            // Создаем новый узел
            *promote_node = create_node();
            
            // Левый узел (текущий) получает левую часть
            node->key_count = 1;
            node->keys[0] = temp_keys[0];
            node->children[0] = temp_children[0];
            node->children[1] = temp_children[1];
            
            // Правый узел (новый) получает правую часть
            (*promote_node)->key_count = 1;
            (*promote_node)->keys[0] = temp_keys[2];
            (*promote_node)->children[0] = temp_children[2];
            (*promote_node)->children[1] = temp_children[3];
            
            // Обновляем родителей
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
    
    // Отступы
    for (int i = 0; i < level-1; i++) {
        printf("  ");
    }
    if (level) {
        printf("->");
    }

    // Печатаем ключи узла
    printf("[");
    for (int i = 0; i < root->key_count; i++) {
        printf("%d", root->keys[i]);
        if (i < root->key_count - 1) printf(", ");
    }
    printf("]\n");
    
    // Рекурсивно печатаем детей
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
