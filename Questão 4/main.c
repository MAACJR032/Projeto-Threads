#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#define NUM_THREADS 27

pthread_mutex_t mutex_banco = PTHREAD_MUTEX_INITIALIZER;

bool valido = true;

int sudoku[9][9] = { {5, 3, 4, 6, 7, 8, 9, 1, 2},
                     {6, 7, 2, 1, 9, 5, 3, 4, 8},
                     {1, 9, 8, 3, 4, 2, 5, 6, 7},
                     {8, 5, 9, 7, 6, 1, 4, 2, 3},
                     {4, 2, 6, 8, 5, 3, 7, 9, 1},
                     {7, 1, 3, 9, 2, 4, 8, 5, 6},
                     {9, 6, 1, 5, 3, 7, 2, 8, 4},
                     {2, 8, 7, 4, 1, 9, 6, 3, 5},
                     {3, 4, 5, 2, 8, 6, 1, 7, 9} };

void* LC(void *idfunc)
{
    int *id = (int *) idfunc;

    int linha = id[0];
    int coluna = id[1];

    if (coluna == 0)
    {
        linha -= 1;

        bool nums[10] = {false};

        for (int j = 0; j < 9 && valido; j++)
        {        
            int num = sudoku[linha][j];
            
            if (nums[num])
            {
                pthread_mutex_lock(&mutex_banco);
                valido = false;
                pthread_mutex_unlock(&mutex_banco);
            }

            nums[num] = true;        
        }
    } 
    else
    {
        coluna -= 1;

        bool nums[10] = {false};

        for (int i = 0; i < 9 && valido; i++)
        {
            int num = sudoku[i][coluna];
            
            if(nums[num]){

                pthread_mutex_lock(&mutex_banco);
                valido = false;
                pthread_mutex_unlock(&mutex_banco);
            }

            nums[num] = true;
        }
    }

    pthread_exit(NULL);
}

void *Quad(void *idfunc)
{
    int *id = (int *) idfunc;

    int linha = id[0];
    int coluna = id[1];

    bool nums[10] = {false}; 

    for (int i = linha; i < linha + 3 && valido; i++)
    {
        for (int j = coluna; j < coluna + 3 && valido; j++)
        {

            int num = sudoku[i][j];

            if (num < 1 || num > 9 || nums[num])
            {
                pthread_mutex_lock(&mutex_banco);
                valido = false;
                pthread_mutex_unlock(&mutex_banco);
            }

            nums[num] = true;
        }
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[NUM_THREADS];

    int cod;
    int threadcount = 0;
    int index[2];

    // linhas
    for (int id = 1; id < 10; id++)
    {
        index[0] = id; // linha
        index[1] = 0; // coluna

        cod = pthread_create(&threads[threadcount], NULL, LC, (void *) index);
        threadcount++;

        if (cod)
            printf("Problema ao criar thread: %d", cod);
    }

    // colunas
    for (int id = 1; id < 10; id++)
    {
        index[0] = 0; // linha
        index[1] = id; // coluna

        cod = pthread_create(&threads[threadcount], NULL, LC, (void *) index);
        threadcount++;

        if (cod)
            printf("Problema ao criar thread: %d", cod);
    }

    // 3x3
    for (int idx = 0; idx < 9; idx += 3)
    {
        for (int idy = 0; idy < 9; idy += 3)
        {
            index[0] = idx;
            index[1] = idy;

            cod = pthread_create(&threads[threadcount], NULL, Quad, (void *) index);

            if (cod)
                printf("Problema ao criar thread: %d", cod);

            threadcount++;
        }
    }

    for (int id = 0; id < NUM_THREADS; id++)
        pthread_join(threads[id], NULL);

    if (valido)
        printf("Solucao valida!\n");
    else
        printf("Solucao invalida!\n");
    
    pthread_exit(NULL);
    return 0;
}
