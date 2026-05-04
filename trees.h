typedef struct Node {
    int keys[2];             
    struct Node* children[3]; 
    int n;                  
    int is_leaf;
}Node;

Node* create_node(int is_leaf);

Node* search(Node* root, int key);

Node* insert(Node* root, int key);

void traverse(Node* root);

void print_tree(Node* root, int level);

void free_tree(Node* root);
