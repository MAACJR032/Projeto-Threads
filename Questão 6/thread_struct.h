#ifndef THREAD_STRUCT_INCLUDED_H
#define THREAD_STRUCT_INCLUDED_H

typedef struct {
    void (*func)(void*);
    void* arg;
} Thread;

#endif
