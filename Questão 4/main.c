#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM 27

typedef struct{

    int linha;
    int coluna;

}Dado;

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

void *LC(void *idfunc) {

    Dado *id = (Dado *) idfunc;

    int linha = id->linha;
    int coluna = id->coluna;

    if (coluna == 0) {

        linha -= 1;

        bool nums[10] = {false};

        for (int j = 0; j < 9; j++) {
                
                int num = sudoku[linha][j];
                
                if(nums[num]){
                    pthread_mutex_lock(&mutex);
                    valido = false;
                    pthread_mutex_unlock(&mutex);
                }

                nums[num] = true;        
        }
    } 
    
    else {

        coluna -= 1;

        bool nums[10] = {false};

        for (int i = 0; i < 9; i++) {
            
            int num = sudoku[i][coluna];
            
            if(nums[num]){

                pthread_mutex_lock(&mutex);
                valido = false;
                pthread_mutex_unlock(&mutex);
            }

            nums[num] = true;
        }
    }

    pthread_exit(NULL);
}

void *Quad(void *idfunc) {

    Dado *id = (Dado *) idfunc;

    int linha = id->linha;
    int coluna = id->coluna;

    bool nums[10] = {false}; 

    for (int i = linha; i < linha + 3; i++) {
        for (int j = coluna; j < coluna + 3; j++) {

            int num = sudoku[i][j];

            if (num < 1 || num > 9 || nums[num]) {
                pthread_mutex_lock(&mutex);
                valido = false;
                pthread_mutex_unlock(&mutex);
            }

            nums[num] = true;
        }
    }

    pthread_exit(NULL);
}

int main() {

    pthread_t Processos[NUM];
    Dado Dados[NUM];

    int cod;
    int threadcount = 0;

    for (int id = 1; id < 10; id++) { // linhas

        Dados[threadcount].linha = id; 
        Dados[threadcount].coluna = 0; 

        cod = pthread_create(&Processos[threadcount], NULL, LC, (void *) &Dados[threadcount]);
        threadcount++;

        if (cod) {
            printf("Problema ao criar thread: %d", cod);
        }
    }

    for (int id = 1; id < 10; id++) { // colunas

        Dados[threadcount].linha = 0; 
        Dados[threadcount].coluna = id; 

        cod = pthread_create(&Processos[threadcount], NULL, LC, (void *) &Dados[threadcount]);
        threadcount++;

        if (cod) {
            printf("Problema ao criar thread: %d", cod);
        }
    }

    for (int idx = 0; idx < 9; idx += 3) { // 3x3
        for (int idy = 0; idy < 9; idy += 3) {

            Dados[threadcount].linha = idx;
            Dados[threadcount].coluna = idy;

            cod = pthread_create(&Processos[threadcount], NULL, Quad, (void *) &Dados[threadcount]);

            if (cod) {
                printf("Problema ao criar thread: %d", cod);
            }

            threadcount++;
        }
    }

    for (int id = 0; id < NUM; id++) {
        pthread_join(Processos[id], NULL);
    }

    if (valido) {
        printf("É uma solução válida\n");
    } 
    
    else {
        printf("Não é uma solução válida\n");
    }

    pthread_exit(NULL);

    return 0;
}
