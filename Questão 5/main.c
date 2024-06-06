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

// Ax = B
int A[2][2] = {{2, 1}, {5, 7}};
int B[2][1] = {{11}, {13}};

// array para guarda os valores antigos de cada variavel 
double X[I][P]; 

void *Jacobi(void *idfunc) {

    Dados *temp = (Dados *)idfunc;
    
    // Esperar as thread para nenhuma ser instanciada durante o algoritmo
    pthread_barrier_wait(&Barreira); 

    for (int k = 0; k < P; k++) {

        int inc = temp->id;

        // while para calcular todas as variaveis da respectiva thread

        while (inc < temp->id + temp->qtd) { 
            
            double sum = 0.0;
            
            // fazer o somatorio de A[ij] * X[j]
            for (int j = 0; j < I; j++) {
                if (j != inc) {
                    sum += A[inc][j] * X[j][k];
                }
            }
            
            // Formula de Jacobi, colocando o valor para X( K + 1)
            X[inc][k + 1] = (1.0 / A[inc][inc]) * (B[inc][0] - sum); 
            
            inc++;
        }

        // esperar todas as threads calcularem para cada K
        pthread_barrier_wait(&Barreira); 
    }

    pthread_exit(NULL);
}

int main() {
    
    // Inicializar com 1 todas as variaveis
    for (int i = 0; i < I; i++) {
        X[i][0] = 1.0;
    }

    int qtd;

    printf("Digite a quantidade de thread: \n");
    scanf("%d", &qtd);

    Dados dado[qtd];
    pthread_t threads[qtd];

    pthread_barrier_init(&Barreira, NULL, qtd);

    //fazer a divisão de qtd variaveis por thread 
    int qtd_por_thread = I / qtd;
    int resto = I % qtd;
    int inc = 0;

    for (int i = 0; i < qtd; i++) {

        //Distribui 1 para cada qtd-1 thread 
        //Caso a divisão seja inexata  
        dado[i].qtd = qtd_por_thread + (i < resto ? 1 : 0); 
        dado[i].id = inc;

        inc += dado[i].qtd;

        int cod = pthread_create(&threads[i], NULL, Jacobi, (void *)&dado[i]);

        if (cod) {
            printf("Erro ao criar thread: %d", cod);
            return 1;
        }
    }

    for (int i = 0; i < qtd; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&Barreira);

    for(int i=0; i < I; i++){
        printf("Solução X%d : %f\n", i, X[i][P]);
    }

    return 0;
}
