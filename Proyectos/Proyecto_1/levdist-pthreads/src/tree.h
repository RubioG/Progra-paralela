#ifndef TREE_H
#define TREE_H
#include <stddef.h>
// Taken from https://www.geeksforgeeks.org/binary-search-tree-set-1-search-and-insertion/
typedef struct node
{
    size_t distance;
    char* files;
    struct node* left;
    struct node* right;
}node;
struct node* new_node(size_t distance, char* files);
void inorder(struct node* root);
struct node* insert(struct node* node, size_t distance, char* files);
#endif // TREE_H
