#ifndef QUEUE_INCLUDED_H
#define QUEUE_INCLUDED_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "thread_struct.h"

typedef struct Node
{
    Thread thread;
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

void enqueue(Queue *q, void (*func)(void*), void* arg)
{
    Node *newNode = (Node*) malloc(sizeof(Node));

    newNode->thread = (Thread){func, arg};
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
#endif
