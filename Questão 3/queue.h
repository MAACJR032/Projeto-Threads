#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// operation pode receber 2 valore: 0 e 1
// 0 - depósito
// 1 - saque
typedef struct Node
{
    int value;
    int operation;
    struct Node *next;
} Node;

typedef struct Queue
{
    Node *head;
    Node *tail;
    int size;
} Queue;

Queue *create_queue()
{
    Queue *q = (Queue*) malloc(sizeof(Queue));

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;

    return q;
}

int getSize(Queue *q)
{
    return q->size;
}

int peek(Queue *q, bool *status)
{
    if (q->size == 0)
    {
        (*status) = false;
        return 0;
    }

    (*status) = true;
    return q->head->value;
}

void enqueue(Queue *q, int value, int operation)
{
    Node *newNode = (Node*) malloc(sizeof(Node));

    newNode->value = value;
    newNode->operation = operation;
    newNode->next = NULL;

    if (q->size == 0)
    {
        q->head = newNode;
        q->tail = newNode;
    }
    else
    {   
        q->tail->next = newNode;
        q->tail = newNode;
    }
    q->size++;
}

int dequeue(Queue *q)
{
    if (q->size == 0)
        return -1;

    Node *temp = q->head;
    int value = q->head->value;

    if (q->size == 1)
    {
        q->head = NULL;
        q->tail = NULL;
    }
    else
        q->head = q->head->next;    

    free(temp);
    q->size--;
    
    return value;
}

bool is_empty(Queue *q)
{
    if (q->size == 0)
        return true;
    return false;
}
