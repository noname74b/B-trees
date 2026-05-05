#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    int keys[2]; 
    int key_count; 
    struct Node* children[3];
    struct Node* parent;
} Node;


Node* create_node();

bool is_leaf(Node* node);

void insert_key_into_node(Node* node, int key, Node* left_child, Node* right_child);

void split_node(Node* node, int* middle_key, Node** new_node);

bool insert_helper(Node* node, int key, int* promote_key, Node** promote_node);

Node* insert(Node* root, int key);

bool search(Node* root, int key);

int get_min(Node* root);

Node* delete_key(Node* root, int key);

void print_tree(Node* root, int level);

void traverse(Node* root);

void free_tree(Node* root);
