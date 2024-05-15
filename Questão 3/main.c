#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#define NUM_CLIENTS 3
#define MAX_FILE 50

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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
* Nome da operação e valor (se necessário para a operação)
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
    /* Le a operação: Depósito, Saque ou Consulta
     * Verifica se pode ser executada (P/ depósito ou saque)
     * Sendo possível, altera os valores no banco
     * Depois altera o valor no cliente   
    */

    Cliente *client = (Cliente *) cliente;

    FILE *file = fopen((char *) client->client_file, "rt");

    if (file != NULL)
    {
        char operation[1000];
        char *read_operation;
        
        while (fscanf(file, " %999[^ ]", operation) != EOF)
        {
            // Faz um split da linha, ao encontrar esses caracteres
            // E passa a primeira palavra como referência para read_operation
            read_operation = strtok(operation, " \t\n");

            // Lê todas as palavras de uma linha
            while (read_operation != NULL)
            {
                // Põe em minúsculo a operação a ser executada
                lower(read_operation);

                if (strcmp(read_operation, "deposito") == 0)
                {   
                    pthread_mutex_lock(&mutex);

                    // Lê o valor a ser depositado
                    double deposito = 0;
                    fscanf(file, "%lf", &deposito);

                    // Checa se a operação é válida
                    if (deposito > 0)
                    {
                        client->saldo += deposito;
                        banco_saldo += deposito;
                    }

                    pthread_mutex_unlock(&mutex);
                }
                else if (strcmp(read_operation, "saque") == 0)
                {
                    pthread_mutex_lock(&mutex);

                    // Lê o valor a ser sacado
                    double saque = 0;
                    fscanf(file, "%lf\n", &saque);

                    // Checa se a operação é válida
                    if (saque > 0 && client->saldo >= saque && banco_saldo >= saque)
                    {
                        client->saldo -= saque;
                        banco_saldo -= saque;
                    }

                    pthread_mutex_unlock(&mutex);
                }
                else if (strcmp(read_operation, "consulta") == 0)
                {   
                    pthread_mutex_lock(&mutex);

                    printf("Saldo do Cliente %d: %.2lf\n", client->id, client->saldo);

                    pthread_mutex_unlock(&mutex);
                }

                // Pegar a referência da próxima palavra
                read_operation = strtok(NULL, " \t\n");
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