#include "tree.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stddef.h>

// A utility function to create a new BST node
struct node* new_node(size_t distance, char* files)
{
    struct node *temp =  (struct node *)malloc(sizeof(struct node));
    temp->distance = distance;
    temp->left = temp->right = NULL;
    temp->files = files;
    return temp;
}

// A utility function to do inorder traversal of BST
void inorder(struct node *root)
{
    if (root != NULL)
    {
        inorder(root->left);
        printf("%ld\t%s\n", root->distance, root->files);
        free(root->files);
        inorder(root->right);
        free(root);
    }
}

/* A utility function to insert a new node with given key in BST */
struct node* insert(struct node* node, size_t distance, char* files)
{
    /* If the tree is empty, return a new node */
    if (node == NULL) return new_node(distance, files);
    /* Otherwise, recur down the tree */
    if (distance < node->distance)
        node->left  = insert(node->left, distance, files);
    else if (distance > node->distance)
        node->right = insert(node->right, distance, files);
    else if(strcmp(node->files, files) < 0)
        node->right = insert(node->right, distance, files);
    else
        node->left = insert(node->left, distance, files);

    /* return the (unchanged) node pointer */
    return node;
}
