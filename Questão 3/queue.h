#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// operation pode receber 3 valores: 0, 1 e 2
// 0 - depósito
// 1 - saque
// 2 - consulta
typedef struct Node
{
    double value;
    int operation;
    int id;
    struct Node *next;
} Node;

typedef struct Queue
{
    Node *head;
    Node *tail;
    int size;
} Queue;

Queue* create_queue()
{
    Queue *q = (Queue*) malloc(sizeof(Queue));

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;

    return q;
}

void enqueue(Queue *q, double value, int operation, int id)
{
    Node *newNode = (Node*) malloc(sizeof(Node));

    newNode->value = value;
    newNode->operation = operation;
    newNode->id = id;
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

Node* dequeue(Queue *q)
{
    if (q->size == 0)
        return NULL;

    Node *temp = q->head;

    if (q->size == 1)
    {
        q->head = NULL;
        q->tail = NULL;
    }
    else
        q->head = q->head->next;    

    q->size--;
    
    return temp;
}

bool is_empty(Queue *q)
{
    if (q->size == 0)
        return true;
    return false;
}

void destroy_queue(Queue *q)
{
    Node *curr = q->head;

    while (curr != NULL)
    {
        Node *temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(q);   
}