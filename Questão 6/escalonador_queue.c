#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

#define N 4 // Numero maximo de threads executando simultaneamente

// garante que apenas uma thread por vez acesse a lista
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// condicional para fazer a thread do escalonador dormir quando n tem thread na lista e acorda quando uma for inserida
pthread_cond_t emptyConditional = PTHREAD_COND_INITIALIZER;

// condicional para fazer a thread do escalonador dormir quando n tem thread na lista e acorda quando uma for inserida
pthread_cond_t fullConditional = PTHREAD_COND_INITIALIZER;

pthread_t threads_exec[N];

// lista_pronto --> array que representa uma fila de execucao pendente das threads
// estrutra que tem um ponteiro para a funcao e cada thread da lista e representado por essa estrutura
Queue *lista_pronto;
int count = 0;

void* executar(void *arg)
{
    Node *node = (Node*) arg;
    node->thread.func(node->thread.arg);
    free(node);

    // Avisa para o escalonador que uma thread terminou
    // E que há um núcleo disponivel
    pthread_mutex_lock(&mutex);

    count--;
    pthread_cond_signal(&fullConditional);
    
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

// entra em loop infinito , bloqueia o mutex para garantir acesso exclusivo a lista
void* escalonador(void* arg)
{
    while (true)
    {
        pthread_mutex_lock(&mutex);

        // se a lista tiver vazia , espera o condicional fazendo com que a thread do escalonador durma ate uma nova ser inserida
        while(lista_pronto->size == 0)
        {
            pthread_cond_wait(&emptyConditional, &mutex);
        }

        // Enquanto tiver thread na lista coloca pra executar
        while(lista_pronto->size > 0)
        {
            // Enquanto os N núcleos estiverem cheios escalonador dorme
            while(count == N)
            {
                pthread_cond_wait(&fullConditional, &mutex);
            }

            pthread_t tmp;
            Node *node = dequeue(lista_pronto);
            pthread_create(&tmp, NULL, executar, node);
            count++;
        }

        pthread_mutex_unlock(&mutex);
    }
}

// adiciona uma nova thread a lista
void agendar(void (*func)(void*), void* arg)
{
    pthread_mutex_lock(&mutex);//bloqueia o mutex e garante acesso exclusivo a lista

    enqueue(lista_pronto, func, arg);

    // avisa a cond para acordar a thread do escalonador
    pthread_cond_signal(&emptyConditional);
    pthread_mutex_unlock(&mutex);
}

void funcao_qualquer(void* arg)
{
    printf("Thread executou\n");
}

int main()
{
    lista_pronto = create_queue();
    

    // cria a thread do escalonador e inicia a func escalonador
    pthread_t thread_escalonador;
    pthread_create(&thread_escalonador, NULL, escalonador, NULL);

    // Adicione threads a lista_pronto usando a funcao agendar
    for(int i = 0; i < 4*N; i++)
        agendar(funcao_qualquer, NULL);
    
    // espera a thread do escalonador terminar(pthread_join) sendo que como ela esta em loop infinito n vai terminar
    // pthread_join e apenas para evitar que termine antes do esperado
    pthread_join(thread_escalonador, NULL);
    destroy_queue(lista_pronto);
    return 0;
}
