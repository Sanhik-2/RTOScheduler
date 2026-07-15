#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Adelson_RT.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))

// --- 2. MEMORY FACTORIES ---
process_metadata_t* createProcess(uint32_t pid, int priority, int exec_time) {
    process_metadata_t* pcb = (process_metadata_t*)malloc(sizeof(process_metadata_t));
    pcb->processid = pid;
    pcb->priority = priority;
    pcb->execution_time = exec_time;
    pcb->time_remaining = exec_time;
    pcb->state = HASNTSTARTED;
    return pcb;
}

Node* createNode(process_metadata_t* process_n) {
    Node* task_n = (Node *)malloc(sizeof(Node));
    task_n->pcb = process_n;
    task_n->left = NULL;
    task_n->right = NULL;
    task_n->height = 1;
    return task_n;
}

// --- 3. AVL TREE ENGINE ---
int height(Node* node) {
    return node ? node->height : 0;
}

int getBalance(Node* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = 1 + max(height(y->left), height(y->right));
    x->height = 1 + max(height(x->left), height(x->right));
    return x;
}

Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = 1 + max(height(x->left), height(x->right));
    y->height = 1 + max(height(y->left), height(y->right));
    return y;
}

Node* insertProcess(Node* node, process_metadata_t* process_n) {
    if (node == NULL) return createNode(process_n);

    // Primary Sort: Priority
    if (process_n->priority < node->pcb->priority) {
        node->left = insertProcess(node->left, process_n);
    } 
    else if (process_n->priority > node->pcb->priority) {
        node->right = insertProcess(node->right, process_n);
    } 
    // Secondary Sort: PID Tie-Breaker
    else {
        if (process_n->processid < node->pcb->processid) {
            node->left = insertProcess(node->left, process_n);
        } else if (process_n->processid > node->pcb->processid) {
            node->right = insertProcess(node->right, process_n);
        } else {
            return node; // Absolute duplicate
        }
    }

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    // Rebalance using child balance factors for safety
    if (balance > 1 && getBalance(node->left) >= 0)
        return rightRotate(node);
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0)
        return leftRotate(node);
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node; 
}

Node* extractMinProcess(Node* root, process_metadata_t** extracted_pcb) {
    if (root == NULL) {
        *extracted_pcb = NULL;
        return NULL;
    }

    // Traverse to absolute highest priority task
    if (root->left != NULL) {
        root->left = extractMinProcess(root->left, extracted_pcb);
    } else {
        *extracted_pcb = root->pcb;
        Node* temp = root->right;
        free(root);
        return temp; 
    }

    root->height = 1 + max(height(root->left), height(root->right));
    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0) return rightRotate(root);
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0) return leftRotate(root);
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}

// Optional: Keep inorder to print the Ready Queue state if needed
void printQueue(Node* root) {
    if (root != NULL) {
        printQueue(root->left);
        printf("[PID:%u P:%d] ", root->pcb->processid, root->pcb->priority);
        printQueue(root->right);
    }
}



