#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <pthread.h>

#define I 2
#define P 10

typedef struct {
    int id;
    int qtd;
} Dados;

pthread_barrier_t Barreira;

//Ax = B

int A[2][2] = {{2, 1}, {5, 7}};
int B[2][1] = {{11}, {13}};

double X[I][P]; //Guarda os valores antigos de cada X

void *Jacobi(void *idfunc) {

    Dados *temp = (Dados *)idfunc;

    pthread_barrier_wait(&Barreira); // Esperar as thread para nenhuma ser instanciada durante o algoritmo

    for (int k = 0; k < P; k++) {

        int inc = temp->id;

        while (inc < temp->id + temp->qtd) { // while para calcular todas os xi da thread 

            double sum = 0.0;
            
            for (int j = 0; j < I; j++) {

                if (j != inc) {
                    sum += A[inc][j] * X[j][k];
                }
            }
            
            X[inc][k + 1] = (1.0 / A[inc][inc]) * (B[inc][0] - sum); 
            
            inc++;
        }

        pthread_barrier_wait(&Barreira); // esperar todas as threads calcularem para cada K
    }

    pthread_exit(NULL);
}

int main() {

    for (int i = 0; i < I; i++) { //Inicializar com 1
        X[i][0] = 1.0;
    }

    int qtd;

    printf("Digite a quantidade de thread: \n");
    scanf("%d", &qtd);

    Dados dado[qtd];
    pthread_t Processos[qtd];

    pthread_barrier_init(&Barreira, NULL, qtd);

    int qtd_por_thread = I / qtd;
    int resto = I % qtd;
    int inc = 0;

    for (int i = 0; i < qtd; i++) {

        dado[i].qtd = qtd_por_thread + (i < resto ? 1 : 0); //Distribui 1 para cada qtd-1 thread
        dado[i].id = inc;

        inc += dado[i].qtd;

        int cod = pthread_create(&Processos[i], NULL, Jacobi, (void *)&dado[i]);

        if (cod) {
            printf("Erro ao criar thread: %d", cod);
            return 1;
        }
    }

    for (int i = 0; i < qtd; i++) {
        pthread_join(Processos[i], NULL);
    }

    pthread_barrier_destroy(&Barreira);

    for(int i=0; i < I; i++){

        printf("Solução X%d : %f\n", i, X[i][P]);

    }

    return 0;
}
