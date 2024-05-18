#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include "queue.h"

#define NUM_CLIENTS 3
#define NUM_FILES 4
#define MAX_FILE 50
#define MAX_LINE 100

pthread_mutex_t mutex_banco = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_clientes[NUM_CLIENTS]; 
pthread_cond_t cond_banco = PTHREAD_COND_INITIALIZER;

// garante-se que o saldo inicial do banco é maior ou igual a soma dos saldos iniciais dos clientes
double banco_saldo = 100000000;
double clientes_saldo[NUM_CLIENTS] = {1500, 2500, 15000};
Queue *solicitacoes;

/*
* Funcionamento do txt:
* 
* Primeira linha contém o id do cliente, que é usado para acessar o array clientes_saldo
* O id está em 0 <= id < NUM_CLIENTS
*
* As outras linhas terão 1 operação por linha
* Em cada linha haverá: Nome da operação e se necessário o valor
* Todas as operações presente nos arquivos txt são apenas depósito, saque e consulta
*
*/


// Função para colocar palavra em minúsculo
void lower(char *read_word)
{
    for (int i = 0; i < strlen(read_word); i++)
        read_word[i] = tolower(read_word[i]);
}

void *client_operation(void *filename)
{
    FILE *file = fopen((char *) filename, "rt");

    if (file != NULL)
    {
        // Pegar o id do cliente
        int id = 0;
        fscanf(file, "%d\n", &id);

        // Vai ler linha por linha
        char line[MAX_LINE];
    
        while (fgets(line, sizeof(line), file) != NULL)
        {
            char operation[MAX_LINE];
            double value = 0;

            /* 
            * Vai ler operação e valor da linha
            * Se for lido apenas a operação (Único caso que ocorre é o da consulta)
            * vai mostrar o saldo, já que o valor de retorno vai ser 1
            */
            if (sscanf(line, "%s %lf", operation, &value) != 2)
            {
                pthread_mutex_lock(&mutex_clientes[id]);
                printf("Saldo do Cliente %d: %.2lf\n", id, clientes_saldo[id]);
                pthread_mutex_unlock(&mutex_clientes[id]);

                continue;
                // destravar
            }
     
            lower(operation);

            if (strcmp(operation, "deposito") == 0)
            {
                // Checa se a operação é válida
                if (value >= 0)
                {
                    // Coloca uma solicitação de depósito na fila
                    pthread_mutex_lock(&mutex_banco);

                    // Adiciona na fila
                    enqueue(solicitacoes, value, 0);

                    pthread_cond_signal(&cond_banco); // Sinaliza a thread do banco que há uma nova solicitação

                    pthread_mutex_unlock(&mutex_banco);

                    // Atualiza o saldo do cliente
                    pthread_mutex_lock(&mutex_clientes[id]);
                    clientes_saldo[id] += value;
                    pthread_mutex_unlock(&mutex_clientes[id]);
                }

            }
            else if (strcmp(operation, "saque") == 0)
            {
                // Checa se a operação é válida
                if (value >= 0 && clientes_saldo[id] >= value)
                {
                    pthread_mutex_lock(&mutex_banco);

                    // Adiciona na fila
                    enqueue(solicitacoes, value, 1);

                    pthread_cond_signal(&cond_banco); // Sinaliza a thread do banco que há uma nova solicitação

                    pthread_mutex_unlock(&mutex_banco);

                    // Atualiza o saldo do cliente
                    pthread_mutex_lock(&mutex_clientes[id]);
                    clientes_saldo[id] -= value;
                    pthread_mutex_unlock(&mutex_clientes[id]);
                }
            }
        }
        fclose(file);
    }
    else
        printf("Thread failed to open file %s\n", (char *) filename);
}

void* bank_operations()
{
    while (1)
    {
        pthread_mutex_lock(&mutex_banco);

        while (is_empty(solicitacoes))
        {
            pthread_cond_wait(&cond_banco, &mutex_banco);
        }

        // Atualiza os valores do banco
        if (solicitacoes->tail->operation == 0)
            banco_saldo += solicitacoes->tail->value;
        else
            banco_saldo -= solicitacoes->tail->value;
        
        // Remove da fila
        dequeue(solicitacoes);

        pthread_mutex_unlock(&mutex_banco);
    }
}

int main()
{
    solicitacoes = create_queue();
    pthread_t threads[NUM_FILES+1];
    char files[NUM_FILES][MAX_FILE] = {"cliente_0.txt", 
                                       "cliente_1.txt",
                                       "cliente_2.txt",
                                       "cliente_3.txt"};
    
    /* REVER LÓGICA DO SINAL */

    // Inicializando os mutexes dos clientes
    for (int i = 0; i < NUM_CLIENTS; i++)
        pthread_mutex_init(&mutex_clientes[i], NULL);

    /* FAZER A THREAD DO BANCO */
    
    // Inicializando o mutex de cada cliente
    for (int i = 0; i < NUM_FILES; i++)
        mutex_cliente[i] = PTHREAD_MUTEX_INITIALIZER;
    
    // Threads dos clientes
    for (int i = 0; i < NUM_FILES; i++)
        pthread_create(&threads[i], NULL, client_operation, (void *) files[i]);


    pthread_create(&threads[NUM_CLIENTS], NULL, bank_operations, NULL);

    for (int i = 0; i < NUM_FILES + 1; i++)
        pthread_join(threads[i], NULL);
    
    // Mostrando saldo final dos clientes e do banco
    for (int i = 0; i < NUM_CLIENTS; i++)
        printf("Cliente %d: %.2lf\n", i, clientes_saldo[i]);

    printf("banco: %.2lf\n", banco_saldo);
    
    // Destroindo os mutexes dos clientes
    for (int i = 0; i < NUM_CLIENTS; i++)
        pthread_mutex_destroy(&mutex_clientes[i]);

    return 0;
}
