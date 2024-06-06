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

// funcão para tratar as linhas e colunas
void *LC(void *idfunc) 
{
    Dado *id = (Dado *) idfunc;

    int linha = id->linha;
    int coluna = id->coluna;

    if (linha >= 0) // separar as linhas das colunas
    {
        // array para fazer verificar se houver repetição
        bool nums[10] = {false};

        for (int j = 0; j < 9; j++) 
        {
            int num = sudoku[linha][j];
            
            // se houve repeticao o valor sera true e entrará nesse if
            if (nums[num])
            {
                pthread_mutex_lock(&mutex);
                valido = false;
                pthread_mutex_unlock(&mutex);
            }

            //se n houve coloca como true
            nums[num] = true;    
                
        }
    } 

    else
    {
        // mesma coisa da de cima, porém para tratar as colunas
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

//função para tratar os subgrades 3x3
void *Quad(void *idfunc) 
{
    Dado *id = (Dado *) idfunc;

    int linha = id->linha;
    int coluna = id->coluna;

    // array para verificar repetição
    bool nums[10] = {false}; 

    for (int i = linha; i < linha + 3; i++) 
    {
        for (int j = coluna; j < coluna + 3; j++) 
        {
            int num = sudoku[i][j];

            // se houver repetição ou o numero não estiver no intervalo 1 a 9 entra nesse if 
            if (num < 1 || num > 9 || nums[num]) 
            {
                pthread_mutex_lock(&mutex);
                valido = false;
                pthread_mutex_unlock(&mutex);
            }

            nums[num] = true; // se n houve repetição, modifica para true para dizer que o num já foi contado
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
        Dados[threadcount].linha = id; //oassando o num da linha para cada thread
        Dados[threadcount].coluna = -1; 

        pthread_create(&threads[threadcount], NULL, LC, (void *) &Dados[threadcount]);
        threadcount++;
    }

    // colunas
    for (int id = 0; id < 9; id++) 
    {
        Dados[threadcount].linha = -1; 
        Dados[threadcount].coluna = id; // passando o num da coluna para cada thread 

        pthread_create(&threads[threadcount], NULL, LC, (void *) &Dados[threadcount]);
        threadcount++;
    }

    // 3x3
    // o for somando +3 em x e y é para passar os subgrades para cada thread
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
