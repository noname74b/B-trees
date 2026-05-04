#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ORDER 3  // Порядок B-дерева (минимум ORDER ключей, максимум 2*ORDER ключей)
#define MAX_KEYS (2 * ORDER)
#define MIN_KEYS ORDER

typedef struct BTreeNode {
    int keys[MAX_KEYS + 1];           // Ключи узла (индексация с 1)
    int count;                         // Количество ключей в узле
    struct BTreeNode* children[MAX_KEYS + 2]; // Указатели на дочерние узлы
    bool is_leaf;                      // Является ли узел листом
} BTreeNode;

// Прототипы функций
BTreeNode* create_node(bool is_leaf);
void traverse(BTreeNode* root);
BTreeNode* search(BTreeNode* root, int key);
BTreeNode* insert(BTreeNode* root, int key);
void insert_non_full(BTreeNode* node, int key);
void split_child(BTreeNode* parent, int index, BTreeNode* child);
BTreeNode* delete(BTreeNode* root, int key);
int find_key(BTreeNode* node, int key);
void delete_from_leaf(BTreeNode* node, int index);
void delete_from_non_leaf(BTreeNode* node, int index);
int get_pred(BTreeNode* node, int index);
int get_succ(BTreeNode* node, int index);
void fill(BTreeNode* node, int index);
void borrow_from_prev(BTreeNode* node, int index);
void borrow_from_next(BTreeNode* node, int index);
void merge(BTreeNode* node, int index);
void free_tree(BTreeNode* root);
void print_tree(BTreeNode* root, int level);

// Создание нового узла
BTreeNode* create_node(bool is_leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->is_leaf = is_leaf;
    node->count = 0;
    for (int i = 0; i <= MAX_KEYS + 1; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// Обход дерева в порядке возрастания
void traverse(BTreeNode* root) {
    if (root != NULL) {
        for (int i = 1; i <= root->count; i++) {
            if (!root->is_leaf) {
                traverse(root->children[i]);
            }
            printf("%d ", root->keys[i]);
        }
        if (!root->is_leaf) {
            traverse(root->children[root->count + 1]);
        }
    }
}

// Поиск ключа в B-дереве
BTreeNode* search(BTreeNode* root, int key) {
    if (root == NULL) return NULL;
    
    int i = 1;
    while (i <= root->count && key > root->keys[i]) {
        i++;
    }
    
    if (i <= root->count && key == root->keys[i]) {
        return root;  // Ключ найден
    }
    
    if (root->is_leaf) {
        return NULL;  // Ключ не найден
    }
    
    return search(root->children[i], key);
}

// Разделение дочернего узла
void split_child(BTreeNode* parent, int index, BTreeNode* child) {
    // Создаем новый узел
    BTreeNode* new_node = create_node(child->is_leaf);
    new_node->count = ORDER;
    
    // Копируем последние ORDER ключей в новый узел
    for (int j = 1; j <= ORDER; j++) {
        new_node->keys[j] = child->keys[j + ORDER + 1];
    }
    
    // Если не лист, копируем и дочерние указатели
    if (!child->is_leaf) {
        for (int j = 1; j <= ORDER + 1; j++) {
            new_node->children[j] = child->children[j + ORDER + 1];
        }
    }
    
    child->count = ORDER;
    
    // Сдвигаем элементы в родителе для вставки нового ключа
    for (int j = parent->count + 1; j >= index + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[index + 1] = new_node;
    
    for (int j = parent->count; j >= index; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    
    // Вставляем средний ключ в родительский узел
    parent->keys[index] = child->keys[ORDER + 1];
    parent->count++;
}

// Вставка в неполный узел
void insert_non_full(BTreeNode* node, int key) {
    int i = node->count;
    
    if (node->is_leaf) {
        // Вставка в листовой узел
        while (i >= 1 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->count++;
    } else {
        // Вставка во внутренний узел
        while (i >= 1 && key < node->keys[i]) {
            i--;
        }
        i++;
        
        if (node->children[i]->count == MAX_KEYS) {
            split_child(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insert_non_full(node->children[i], key);
    }
}

// Основная функция вставки
BTreeNode* insert(BTreeNode* root, int key) {
    if (root == NULL) {
        root = create_node(true);
        root->keys[1] = key;
        root->count = 1;
        return root;
    }
    
    if (root->count == MAX_KEYS) {
        // Корень полный, нужно разделить
        BTreeNode* new_root = create_node(false);
        new_root->children[1] = root;
        split_child(new_root, 1, root);
        
        int i = 1;
        if (key > new_root->keys[1]) {
            i++;
        }
        insert_non_full(new_root->children[i], key);
        return new_root;
    } else {
        insert_non_full(root, key);
        return root;
    }
}

// Поиск позиции ключа в узле
int find_key(BTreeNode* node, int key) {
    int idx = 1;
    while (idx <= node->count && key > node->keys[idx]) {
        idx++;
    }
    return idx;
}

// Получить предшественника ключа
int get_pred(BTreeNode* node, int index) {
    BTreeNode* current = node->children[index];
    while (!current->is_leaf) {
        current = current->children[current->count + 1];
    }
    return current->keys[current->count];
}

// Получить преемника ключа
int get_succ(BTreeNode* node, int index) {
    BTreeNode* current = node->children[index + 1];
    while (!current->is_leaf) {
        current = current->children[1];
    }
    return current->keys[1];
}

// Заимствование из предыдущего потомка
void borrow_from_prev(BTreeNode* node, int index) {
    BTreeNode* child = node->children[index];
    BTreeNode* sibling = node->children[index - 1];
    
    // Сдвигаем ключи в child
    for (int i = child->count; i >= 1; i--) {
        child->keys[i + 1] = child->keys[i];
    }
    
    if (!child->is_leaf) {
        for (int i = child->count + 1; i >= 1; i--) {
            child->children[i + 1] = child->children[i];
        }
    }
    
    child->keys[1] = node->keys[index - 1];
    
    if (!child->is_leaf) {
        child->children[1] = sibling->children[sibling->count + 1];
    }
    
    node->keys[index - 1] = sibling->keys[sibling->count];
    child->count++;
    sibling->count--;
}

// Заимствование из следующего потомка
void borrow_from_next(BTreeNode* node, int index) {
    BTreeNode* child = node->children[index];
    BTreeNode* sibling = node->children[index + 1];
    
    child->keys[child->count + 1] = node->keys[index];
    
    if (!child->is_leaf) {
        child->children[child->count + 2] = sibling->children[1];
    }
    
    node->keys[index] = sibling->keys[1];
    
    // Сдвигаем ключи в sibling
    for (int i = 1; i < sibling->count; i++) {
        sibling->keys[i] = sibling->keys[i + 1];
    }
    
    if (!sibling->is_leaf) {
        for (int i = 1; i <= sibling->count; i++) {
            sibling->children[i] = sibling->children[i + 1];
        }
    }
    
    child->count++;
    sibling->count--;
}

// Слияние двух потомков
void merge(BTreeNode* node, int index) {
    BTreeNode* child = node->children[index];
    BTreeNode* sibling = node->children[index + 1];
    
    // Вставляем ключ из родителя
    child->keys[ORDER] = node->keys[index];
    
    // Копируем ключи из sibling
    for (int i = 1; i <= sibling->count; i++) {
        child->keys[i + ORDER] = sibling->keys[i];
    }
    
    // Копируем дочерние указатели
    if (!child->is_leaf) {
        for (int i = 1; i <= sibling->count + 1; i++) {
            child->children[i + ORDER] = sibling->children[i];
        }
    }
    
    // Сдвигаем ключи в родителе
    for (int i = index; i < node->count; i++) {
        node->keys[i] = node->keys[i + 1];
    }
    
    // Сдвигаем дочерние указатели
    for (int i = index + 1; i <= node->count; i++) {
        node->children[i] = node->children[i + 1];
    }
    
    child->count += sibling->count + 1;
    node->count--;
    
    free(sibling);
}

// Заполнение потомка, если в нем меньше MIN_KEYS ключей
void fill(BTreeNode* node, int index) {
    if (index > 1 && node->children[index - 1]->count > MIN_KEYS) {
        borrow_from_prev(node, index);
    } else if (index <= node->count && node->children[index + 1]->count > MIN_KEYS) {
        borrow_from_next(node, index);
    } else {
        if (index <= node->count) {
            merge(node, index);
        } else {
            merge(node, index - 1);
        }
    }
}

// Удаление ключа из листового узла
void delete_from_leaf(BTreeNode* node, int index) {
    for (int i = index; i < node->count; i++) {
        node->keys[i] = node->keys[i + 1];
    }
    node->count--;
}

// Удаление ключа из нелистового узла
void delete_from_non_leaf(BTreeNode* node, int index) {
    int key = node->keys[index];
    
    if (node->children[index]->count > MIN_KEYS) {
        int pred = get_pred(node, index);
        node->keys[index] = pred;
        delete(node->children[index], pred);
    } else if (node->children[index + 1]->count > MIN_KEYS) {
        int succ = get_succ(node, index);
        node->keys[index] = succ;
        delete(node->children[index + 1], succ);
    } else {
        merge(node, index);
        delete(node->children[index], key);
    }
}

// Основная функция удаления
BTreeNode* delete(BTreeNode* root, int key) {
    if (root == NULL) {
        printf("Дерево пусто\n");
        return NULL;
    }
    
    int idx = find_key(root, key);
    
    if (idx <= root->count && root->keys[idx] == key) {
        if (root->is_leaf) {
            delete_from_leaf(root, idx);
        } else {
            delete_from_non_leaf(root, idx);
        }
    } else {
        if (root->is_leaf) {
            printf("Ключ %d не найден в дереве\n", key);
            return root;
        }
        
        bool is_last = (idx == root->count + 1);
        
        if (root->children[idx]->count <= MIN_KEYS) {
            fill(root, idx);
        }
        
        if (is_last && idx > root->count + 1) {
            delete(root->children[idx - 1], key);
        } else {
            delete(root->children[idx], key);
        }
    }
    
    // Если корень стал пустым
    if (root->count == 0) {
        BTreeNode* temp = root;
        if (root->is_leaf) {
            root = NULL;
        } else {
            root = root->children[1];
        }
        free(temp);
    }
    
    return root;
}

// Визуализация дерева
void print_tree(BTreeNode* root, int level) {
    if (root != NULL) {
        printf("Уровень %d [ ", level);
        for (int i = 1; i <= root->count; i++) {
            printf("%d ", root->keys[i]);
        }
        printf("] (лист: %s)\n", root->is_leaf ? "да" : "нет");
        
        if (!root->is_leaf) {
            for (int i = 1; i <= root->count + 1; i++) {
                if (root->children[i] != NULL) {
                    printf("  Потомок %d: ", i);
                    print_tree(root->children[i], level + 1);
                }
            }
        }
    }
}

// Освобождение памяти
void free_tree(BTreeNode* root) {
    if (root != NULL) {
        if (!root->is_leaf) {
            for (int i = 1; i <= root->count + 1; i++) {
                free_tree(root->children[i]);
            }
        }
        free(root);
    }
}

// Тестовая программа
int main() {
    BTreeNode* root = NULL;
    int choice, key;
    
    while (1) {
        printf("\n=== B-ДЕРЕВО (порядок %d) ===\n", ORDER);
        printf("1. Вставить ключ\n");
        printf("2. Удалить ключ\n");
        printf("3. Найти ключ\n");
        printf("4. Обход дерева\n");
        printf("5. Показать структуру\n");
        printf("6. Выйти\n");
        printf("Выберите действие: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("Введите ключ для вставки: ");
                scanf("%d", &key);
                root = insert(root, key);
                printf("Ключ %d вставлен\n", key);
                break;
                
            case 2:
                printf("Введите ключ для удаления: ");
                scanf("%d", &key);
                root = delete(root, key);
                break;
                
            case 3:
                printf("Введите ключ для поиска: ");
                scanf("%d", &key);
                if (search(root, key) != NULL) {
                    printf("Ключ %d найден в дереве\n", key);
                } else {
                    printf("Ключ %d не найден\n", key);
                }
                break;
                
            case 4:
                printf("Обход дерева: ");
                traverse(root);
                printf("\n");
                break;
                
            case 5:
                printf("\nСтруктура B-дерева:\n");
                print_tree(root, 0);
                break;
                
            case 6:
                free_tree(root);
                printf("Программа завершена\n");
                return 0;
                
            default:
                printf("Неверный выбор!\n");
        }
    }
    
    return 0;
}