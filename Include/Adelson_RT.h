#ifndef ADELSON_RT_H
#define ADELSON_RT_H

#include <stdint.h>
#include <stdlib.h>

// --- 1. SYSTEM STRUCTURES ---

typedef enum {
    COMPLETED,
    INPROGRESS,
    FAILED,
    STALLED,
    HASNTSTARTED
} process_t;

typedef struct {
    uint32_t processid;
    int priority;           // Lower integer = Higher Priority
    int execution_time;
    int time_remaining;
    process_t state;
} process_metadata_t;

typedef struct Node {
    process_metadata_t* pcb;  
    struct Node *left, *right;
    int height;
} Node;

// --- 2. MEMORY FACTORIES ---

process_metadata_t* createProcess(uint32_t pid, int priority, int exec_time);
Node* createNode(process_metadata_t* process_n);

// --- 3. AVL TREE ENGINE ---

int height(Node* node);
int getBalance(Node* node);
Node* rightRotate(Node* y);
Node* leftRotate(Node* x);

// Core Scheduler Mechanics
Node* insertProcess(Node* node, process_metadata_t* process_n);
Node* extractMinProcess(Node* root, process_metadata_t** extracted_pcb);

// Utilities
void printQueue(Node* root);

#endif // ADELSON_RT_H