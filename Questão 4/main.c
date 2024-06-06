#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM 27

typedef struct {
    int linha;
    int coluna;
} Dado;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool valido = true;

int sudoku[9][9] = {
    {5, 3, 4, 6, 7, 8, 9, 1, 2},
    {6, 7, 2, 1, 9, 5, 3, 4, 8},
    {1, 9, 8, 3, 4, 2, 5, 6, 7},
    {8, 5, 9, 7, 6, 1, 4, 2, 3},
    {4, 2, 6, 8, 5, 3, 7, 9, 1},
    {7, 1, 3, 9, 2, 4, 8, 5, 6},
    {9, 6, 1, 5, 3, 7, 2, 8, 4},
    {2, 8, 7, 4, 1, 9, 6, 3, 5},
    {3, 4, 5, 2, 8, 6, 1, 7, 9}
};

void *LC(void *idfunc) 
{
    Dado *id = (Dado *) idfunc;

    int linha = id->linha;
    int coluna = id->coluna;

    if (linha >= 0) 
    {
        bool nums[10] = {false};

        for (int j = 0; j < 9; j++) 
        {
            int num = sudoku[linha][j];
            
            if (nums[num])
            {
                pthread_mutex_lock(&mutex);
                valido = false;
                pthread_mutex_unlock(&mutex);
            }

            nums[num] = true;        
        }
    } 
    else
    {
        bool nums[10] = {false};

        for (int i = 0; i < 9; i++) 
        {
            int num = sudoku[i][coluna];
            
            if (nums[num]) 
            {
                pthread_mutex_lock(&mutex);
                valido = false;
                pthread_mutex_unlock(&mutex);
            }

            nums[num] = true;
        }
    }

    return NULL;
}

void *Quad(void *idfunc) 
{
    Dado *id = (Dado *) idfunc;

    int linha = id->linha;
    int coluna = id->coluna;

    bool nums[10] = {false}; 

    for (int i = linha; i < linha + 3; i++) 
    {
        for (int j = coluna; j < coluna + 3; j++) 
        {
            int num = sudoku[i][j];

            if (num < 1 || num > 9 || nums[num]) 
            {
                pthread_mutex_lock(&mutex);
                valido = false;
                pthread_mutex_unlock(&mutex);
            }

            nums[num] = true;
        }
    }

    return NULL;
}

int main()
{
    pthread_t threads[NUM];
    Dado Dados[NUM];

    int threadcount = 0;

    // linhas
    for (int id = 0; id < 9; id++) 
    {
        Dados[threadcount].linha = id; 
        Dados[threadcount].coluna = -1; 

        pthread_create(&threads[threadcount], NULL, LC, (void *) &Dados[threadcount]);
        threadcount++;
    }

    // colunas
    for (int id = 0; id < 9; id++) 
    {
        Dados[threadcount].linha = -1; 
        Dados[threadcount].coluna = id; 

        pthread_create(&threads[threadcount], NULL, LC, (void *) &Dados[threadcount]);
        threadcount++;
    }

    // 3x3
    for (int idx = 0; idx < 9; idx += 3) 
    {
        for (int idy = 0; idy < 9; idy += 3) 
        {
            Dados[threadcount].linha = idx;
            Dados[threadcount].coluna = idy;

            pthread_create(&threads[threadcount], NULL, Quad, (void *) &Dados[threadcount]);

            threadcount++;
        }
    }

    for (int id = 0; id < NUM; id++) 
        pthread_join(threads[id], NULL);

    if(valido) 
        printf("É uma solução válida\n");
    else
        printf("Não é uma solução válida\n");

    return 0;
}
