#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 4 // Numero maximo de threads executando simultaneamente

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//garante que apenas uma thread por vez acesse a lista
pthread_cond_t condicional = PTHREAD_COND_INITIALIZER;//condicional para fazer a thread do escalonador dormir quando n tem thread na lista e acorda quando uma for inserida

typedef struct {
    void (*func)(void*);
    void* arg;
} Thread;

//lista_pronto --> array que representa uma fila de execucao pendente das threads
//estrutra que tem um ponteiro para a funcao e cada thread da lista e representado por essa estrutura
Thread* lista_pronto[N];
int count = 0;
pthread_t threads[N]; // Array para armazenar as threads em execucao

void* executar(void* arg) {
    Thread* t = (Thread*)arg;
    t->func(t->arg);
    free(t);
    return NULL;
}

//entra em loop infinito , bloqueia o mutex para garantir acesso exclusivo a lista
void* escalonador(void* arg) {
    while(1) {
        pthread_mutex_lock(&mutex);
        //se a lista tiver vazia , espera o condicional fazendo com que a thread do escalonador durma ate uma nova ser inserida
        while(count == 0) {
            pthread_cond_wait(&condicional, &mutex);
        }

        for(int i = 0; i < N; i++) {
            if(threads[i] == 0 && count > 0) { // Verifica se ha espaco no pool de threads
                Thread* t = lista_pronto[--count];//Quando uma nova thread e adicionada a lista,escalonador acorda, remove a thread da lista e executa a funcao dessa thread.
                pthread_create(&threads[i], NULL, executar, t); // Inicia a thread
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

//adiciona uma nova thread a lista
void agendar(void (*func)(void*), void* arg) {
    pthread_mutex_lock(&mutex);//bloqueia o mutex e garante acesso exclusivo a lista
     //se a lista estiver cheia (count == N)fica esperando cond , a thread que chamou a func agendar dorme ate que tenha espaco na lista
    while(count == N) {
        pthread_cond_wait(&condicional, &mutex);
    }

    Thread* t = (Thread*)malloc(sizeof(Thread));
    t->func = func;
    t->arg = arg;
    lista_pronto[count++] = t;//quando tem espaco na lista a funcao adiciona a nova thread a lista
    //avisa a cond para acordar a thread do escalonador
    pthread_cond_signal(&condicional);
    pthread_mutex_unlock(&mutex);
}

void funcao_qualquer(void* arg) {
    int i;
    for(i = 0; i < 10; i++) {
        printf("Thread executando: %d\n", i);
    }
}

int main() {
    //cria a thread do escalonador e inicia a func escalonador
    pthread_t thread_escalonador;
    pthread_create(&thread_escalonador, NULL, escalonador, NULL);

    for(int i = 0; i < N; i++) {
        // Adicione threads a lista_pronto usando a funcao agendar
        agendar(funcao_qualquer, NULL);
    }

    for(int i = 0; i < N; i++) {
        if(threads[i] != 0) {
            pthread_join(threads[i], NULL); // Espera as threads do pool terminarem
        }
    }
    
    //espera a thread do escalonador terminar(pthread_join) sendo que como ela esta em loop infinito n vai terminar
    //pthread_join e apenas para evitar que termine antes do esperado
    pthread_join(thread_escalonador, NULL);
    return 0;
}
