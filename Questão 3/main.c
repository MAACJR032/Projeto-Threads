#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#define NUM_CLIENTS 3
#define MAX_FILE 50

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
* Por linha: Nome da operação e valor (se necessário para a operação)
* Garante-se que todas as operações nos arquivos txt são
* depósito, saque e consulta 
*/


// Função para colocar palavra em minúsculo
void lower(char *read_word)
{
    for (int i = 0; i < strlen(read_word); i++)
        read_word[i] = tolower(read_word[i]);
}

void *client_operation(void *cliente)
{
    /* 
     * Lê a operação: Depósito, Saque ou Consulta
     * Verifica se pode ser executada (P/ depósito ou saque)
     * Sendo possível, altera os valores do banco
     *  e o valor do cliente   
    */

    Cliente *client = (Cliente *) cliente;

    FILE *file = fopen((char *) client->client_file, "rt");

    if (file != NULL)
    {
        char line[100];

        while (fgets(line, sizeof(line), file) != NULL)
        {
            char operation[1000];
            double value = 0;

            // Ler operação e valor da linha
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

                    pthread_mutex_lock(&mutex);
                    banco_saldo += value;
                    pthread_mutex_unlock(&mutex);
                }

            }
            else if (strcmp(operation, "saque") == 0)
            {
                // Checa se a operação é válida
                if (value >= 0 && client->saldo >= value)
                {
                    client->saldo -= value;
                    
                    pthread_mutex_lock(&mutex);
                    banco_saldo -= value;
                    pthread_mutex_unlock(&mutex);
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
    Cliente cliente[NUM_CLIENTS] = { 1000.00, 1, "cliente_0.txt", 
                                     2000.00, 2, "cliente_1.txt",
                                     15000.00, 3, "cliente_2.txt"};


    // Threads dos clientes
    for (int i = 0; i < NUM_CLIENTS; i++)
        pthread_create(&threads[i], NULL, client_operation, &cliente[i]);

    for (int i = 0; i < NUM_CLIENTS; i++)
        pthread_join(threads[i], NULL);
    
    // Mostrando saldo final dos clientes e do banco
    for (int i = 0; i < NUM_CLIENTS; i++)
        printf("Cliente %d: %.2lf\n", cliente[i].id, cliente[i].saldo);
    
    printf("banco: %.2lf\n", banco_saldo);

    return 0;
}