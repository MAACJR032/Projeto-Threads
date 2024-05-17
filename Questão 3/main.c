#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#define NUM_CLIENTS 3
#define MAX_FILE 50
#define MAX_LINE 100

pthread_mutex_t mutex_banco = PTHREAD_MUTEX_INITIALIZER;

// garante-se que o saldo inicial do banco é maior ou igual a soma dos saldos iniciais dos clientes
double banco_saldo = 100000000;

typedef struct _cliente
{
    double saldo;
    int id;
    char client_file[MAX_FILE];
} Cliente;


/*
* Funcionamento do txt:
* 
* 1 operação por linha
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

void *client_operation(void *cliente)
{
    Cliente *client = (Cliente *) cliente;

    FILE *file = fopen((char *) client->client_file, "rt");

    if (file != NULL)
    {
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
                printf("Saldo do Cliente %d: %.2lf\n", client->id, client->saldo);
                continue;
            }
     
            lower(operation);

            if (strcmp(operation, "deposito") == 0)
            {
                // Checa se a operação é válida
                if (value >= 0)
                {
                    client->saldo += value;

                    pthread_mutex_lock(&mutex_banco);
                    banco_saldo += value;
                    pthread_mutex_unlock(&mutex_banco);
                }

            }
            else if (strcmp(operation, "saque") == 0)
            {
                // Checa se a operação é válida
                if (value >= 0 && client->saldo >= value)
                {
                    client->saldo -= value;
                    
                    pthread_mutex_lock(&mutex_banco);
                    banco_saldo -= value;
                    pthread_mutex_unlock(&mutex_banco);
                }
            }
        }

        fclose(file);
    }
    else
        printf("Thread failed to open file %s\n", client->client_file);
}

int main()
{
    pthread_t threads[NUM_CLIENTS];
    Cliente clients[NUM_CLIENTS] = { 1000.00, 1, "cliente_0.txt", 
                                     2000.00, 2, "cliente_1.txt",
                                     15000.00, 3, "cliente_2.txt"};

    // Threads dos clientes
    for (int i = 0; i < NUM_CLIENTS; i++)
        pthread_create(&threads[i], NULL, client_operation, (void *) &clients[i]);

    for (int i = 0; i < NUM_CLIENTS; i++)
        pthread_join(threads[i], NULL);
    
    // Mostrando saldo final dos clientes e do banco
    for (int i = 0; i < NUM_CLIENTS; i++)
        printf("Cliente %d: %.2lf\n", clients[i].id, clients[i].saldo);

    printf("banco: %.2lf\n", banco_saldo);

    return 0;
}