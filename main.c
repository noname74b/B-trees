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
        printf("5. Демонстрация\n");
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
                printf("\nДемонстрация построения 2-3-дерева:\n");
                free_tree(root);
                root = NULL;
                int numbers[] = {50, 30, 20, 40, 70, 60, 80, 15, 25, 35, 45, 55, 65, 75, 85};
                int n = sizeof(numbers) / sizeof(numbers[0]);
    
                printf("Вставляем числа: ");
                for (int i = 0; i < n; i++) {
                    printf("%d ", numbers[i]);
                }
                printf("\n\n");
                for (int i = 0; i < n; i++) {
                    root = insert(root, numbers[i]);
                    printf("После вставки %d:\n", numbers[i]);
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
