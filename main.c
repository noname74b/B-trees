#include <stdio.h>
#include "trees.h"

// Главная программа
int main() {
    Node* root = NULL;
    int choice, key;
    
    printf("=== 2-3 ДЕРЕВО ===\n");
    printf("2-3-дерево - это сбалансированное дерево поиска,\n");
    printf("где каждый узел может иметь 2 или 3 потомка.\n");
    
    while (1) {
        printf("\nМеню:\n");
        printf("1. Вставить ключ\n");
        printf("2. Найти ключ\n");
        printf("3. Обход дерева\n");
        printf("4. Показать структуру\n");
        printf("5. Сбросить дерево\n");
        printf("6. Демонстрация\n");
        printf("0. Выход\n");
        printf("Выберите: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("Введите ключ: ");
                scanf("%d", &key);
                root = insert(root, key);
                printf("Ключ %d вставлен\n", key);
                break;
                
            case 2:
                printf("Введите ключ для поиска: ");
                scanf("%d", &key);
                if (search(root, key)) {
                    printf("Ключ %d найден!\n", key);
                } else {
                    printf("Ключ %d не найден\n", key);
                }
                break;
                
            case 3:
                printf("Обход дерева: ");
                traverse(root);
                printf("\n");
                break;
                
            case 4:
                printf("\nСтруктура дерева:\n");
                print_tree(root, 0);
                break;

            case 5:
                printf("Дерево сброшено.\n");
                free_tree(root);
                root = NULL;
                break;
                
            case 6:
                printf("\nДемонстрация построения 2-3-дерева:\n");
                free_tree(root);
                root = NULL;
                int demo[] = {10, 20, 30, 40, 50, 15, 25, 35};
                printf("Вставляем числа: ");
                for (int i = 0; i < 8; i++) {
                    printf("%d ", demo[i]);
                }
                printf("\n\n");
                for (int i = 0; i < 8; i++) {
                    root = insert(root, demo[i]);
                    printf("После вставки %d:\n", demo[i]);
                    print_tree(root, 0);
                    printf("\n");
                }
                break;

            case 0:
                free_tree(root);
                printf("Программа завершена\n");
                return 0;
                
            default:
                printf("Неверный выбор!\n");
        }
    }
    
    return 0;
}