#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include "queue.h"

#define NUM_CLIENTS 5
#define NUM_FILES 10
#define MAX_FILE 50
#define MAX_LINE 100

pthread_mutex_t mutex_banco = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_clientes[NUM_CLIENTS]; 
pthread_cond_t cond_banco = PTHREAD_COND_INITIALIZER;
pthread_barrier_t barrier;

// garante-se que o saldo inicial do banco é maior ou igual a soma dos saldos iniciais dos clientes
double banco_saldo = 100000000;
double clientes_saldo[NUM_CLIENTS] = {1500, 2500, 15000, 4200.70, 7120.30};
Queue *operacoes; // Fila de operações
bool fim = false; // Flag para saber se as operações de todos os txt's foram executadas

/*
* Funcionamento do txt:
* 
* Primeira linha contém o id do cliente, que é usado para acessar o array clientes_saldo
* O id varia em: 0 <= id < NUM_CLIENTS
*
* O restante das linhas terão 1 operação por linha
* Nelas haverão: Nome da operação e o valor (com exceção da operação consulta, que não tem o valor)
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
        // Lê o id do cliente (Primeira linha)
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
            * vai adicinar na fila uma operação de consulta, já que o valor de retorno vai ser 1
            */
            if (sscanf(line, "%s %lf", operation, &value) != 2)
            {
                // Adiciona na fila
                // Sinaliza para a thread do banco que há uma nova operação para ser executada
                pthread_mutex_lock(&mutex_banco);
                enqueue(operacoes, 0, 2, id);
                pthread_mutex_unlock(&mutex_banco);

                pthread_cond_signal(&cond_banco); 
                continue;
            }
     
            lower(operation);
            if (strcmp(operation, "deposito") == 0)
            {
                // Checa se é um valor válido
                if (value >= 0)
                {
                    // Adiciona na fila
                    // Sinaliza para a thread do banco que há uma nova operação para ser executada
                    pthread_mutex_lock(&mutex_banco);
                    enqueue(operacoes, value, 0, id);
                    pthread_mutex_unlock(&mutex_banco);

                    pthread_cond_signal(&cond_banco);
                }

            }
            else if (strcmp(operation, "saque") == 0)
            {
                // Checa se é um valor válido
                if (value >= 0)
                {
                    // Adiciona na fila
                    // E Sinaliza para a thread do banco que há uma nova operação para ser executada
                    pthread_mutex_lock(&mutex_banco);
                    enqueue(operacoes, value, 1, id);
                    pthread_mutex_unlock(&mutex_banco);

                    pthread_cond_signal(&cond_banco);
                }
            }
        }
        fclose(file);
    }
    else
        printf("Thread failed to open file %s\n", (char *) filename);
    
    // Espera todas as threads dos clientes terminarem
    pthread_barrier_wait(&barrier);
}

void* bank_operations()
{
    while (true)
    {
        pthread_mutex_lock(&mutex_banco);

        // Espera enquanto a fila de operações está vazia
        // E enquanto todas as threads dos clientes ainda não foram finalizadas
        while (is_empty(operacoes) && fim == false)
        {
            pthread_cond_wait(&cond_banco, &mutex_banco);
        }
        
        // Condição para encerrar a thread do banco
        if (fim == true && is_empty(operacoes))
        {
            pthread_mutex_unlock(&mutex_banco);
            break;    
        }    
        
        // Atualiza os saldos do banco e do cliente
        // E remove a operação da fila
        Node *temp = dequeue(operacoes);

        if (temp != NULL)
        {
            // Trava a thread do cliente, para executar as operações
            pthread_mutex_lock(&mutex_clientes[temp->id]);

            if (temp->operation == 0) // Depósito
            {
                banco_saldo += temp->value;
                clientes_saldo[temp->id] += temp->value;
            }
            else if (temp->operation == 1) // Saque
            {
                // Checa se a operação é válida, para não sacar um valor maior que o saldo da conta
                if (clientes_saldo[temp->id] >= temp->value)
                {
                    banco_saldo -= temp->value;
                    clientes_saldo[temp->id] -= temp->value;
                }
            }
            else if (temp->operation == 2) // Consulta
                printf("Saldo do Cliente %d: %.2lf\n", temp->id, clientes_saldo[temp->id]);

            pthread_mutex_unlock(&mutex_clientes[temp->id]);

            // Libera o nó
            free(temp);
        }
        
        pthread_mutex_unlock(&mutex_banco);
    }
}

int main()
{
    pthread_t threads[NUM_FILES+1];
    char files[NUM_FILES][MAX_FILE] = {"operacoes_0.txt", 
                                       "operacoes_1.txt",
                                       "operacoes_2.txt",
                                       "operacoes_3.txt",
                                       "operacoes_4.txt",
                                       "operacoes_5.txt",
                                       "operacoes_6.txt",
                                       "operacoes_7.txt",
                                       "operacoes_8.txt",
                                       "operacoes_9.txt",};
    
    // Cria a fila
    operacoes = create_queue();

    // Inicializando o mutex dos clientes e a barreira
    for (int i = 0; i < NUM_FILES; i++)
        pthread_mutex_init(&mutex_clientes[i], NULL);
    pthread_barrier_init(barrier, NULL, NUM_FILES);
    
    // Threads dos clientes e do banco
    for (int i = 0; i < NUM_FILES; i++)
        pthread_create(&threads[i], NULL, client_operation, (void *) files[i]);
    pthread_create(&threads[NUM_FILES], NULL, bank_operations, NULL);

    for (int i = 0; i < NUM_FILES; i++)
        pthread_join(threads[i], NULL);

    // Sinalização para encerrar a thread do banco
    fim = true;
    pthread_cond_signal(&cond_banco);

    // Thread do banco
    pthread_join(threads[NUM_FILES], NULL);

    // Mostrando saldo final dos clientes e do banco
    printf("\n");
    for (int i = 0; i < NUM_CLIENTS; i++)
        printf("Cliente %d: %.2lf\n", i, clientes_saldo[i]);
    printf("banco: %.2lf\n", banco_saldo);
    
    // Destruindo os mutexes dos clientes, a barreira e a fila
    for (int i = 0; i < NUM_CLIENTS; i++)
        pthread_mutex_destroy(&mutex_clientes[i]);
    
    pthread_barrier_destroy(&barrier);
    destroy_queue(operacoes);
    
    return 0;
}
