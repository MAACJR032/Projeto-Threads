#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_CLIENTS 5
#define MAX_FILE 50

typedef struct _cliente
{
    int saldo, id;
    char client_file[MAX_FILE];
} Cliente;

void *Operation(void *file_name)
{
    
}

int main()
{
    pthread_t threads[NUM_CLIENTS+1];
    Cliente *cliente;

    // Arquivo com nome, saldo do cliente ? Não precisaria preencher um por um
    cliente = (Cliente *) malloc(sizeof(Cliente) * NUM_CLIENTS);
    for (int i = 0; i < NUM_CLIENTS; i++)
    {
        
        printf("Saldo: ");
        scanf("%d", cliente[i].saldo);
        printf("\n");
    }

    // Criar thread do banco
    for (int i = 0; i < NUM_CLIENTS; i++)
        pthread_create(&threads[i], NULL, Operation, filename);
    
    for (int i = 0; i < NUM_CLIENTS+1; i++)
        pthread_join(threads[i], NULL);
    
    return 0;
}