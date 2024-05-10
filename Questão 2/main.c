#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 5
#define MIN_ARRAY_SIZE 20

// Os argumentos para o bubble sort são o tamanho do array
// e o endereço do primeiro elemento do array
typedef struct
{
    int Size, *ArrayBegin;
} BubbleSortArgs;

void Swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void* BubbleSort(void *args)
{
    BubbleSortArgs *arrayLimits = (BubbleSortArgs *) args;

    for (int i = 0; i < arrayLimits->Size; i++)
    {
        for (int j = 0; j < arrayLimits->Size - i - 1; j++)
        {
            if (arrayLimits->ArrayBegin[j] > arrayLimits->ArrayBegin[j + 1])
                Swap(&arrayLimits->ArrayBegin[j], &arrayLimits->ArrayBegin[j + 1]);
        }
    }

    /* Debugando o bubble sort de cada thread
    for(unsigned int i = 0; i < arrayLimits->Size; i++)
        printf("%i ", arrayLimits->ArrayBegin[i]);
    printf("\n");
    */
}

int main()
{
    int arr[MIN_ARRAY_SIZE] = { 9, 5, 1, 3, 7, 12, 6, 96, 5, 9874, 56, 515, 12, 15, 17, 654, 98, 84, 47, 46 };
    int arrSize = sizeof(arr) / sizeof(int);

    BubbleSortArgs args[MAX_THREADS];
    pthread_t threads[MAX_THREADS];

    unsigned int offset = 0;

    for (unsigned int i = 0; i < MAX_THREADS; i++)
    {
        /* Se o array tiver 101 elementos e dividir por 4 threads
         * todas as threads ficariam com 25 elementos mas a ultima deveria ter na
         * realidade 26 ocasionando um erro nessa forma de dividir os elementos pelas threads
         */
        args[i].ArrayBegin = arr + offset;
        args[i].Size = arrSize / MAX_THREADS;
        offset += args[i].Size;

        pthread_create(&threads[i], NULL, BubbleSort, &args[i]);
    }

    //Esperando todas as threads finalizarem
    for (unsigned int i = 0; i < MAX_THREADS; i++)
        pthread_join(threads[i], NULL);

    //Resultado do array parcialmente ordenado
    for (unsigned int i = 0; i < arrSize; i++)
        printf("%d ", arr[i]);
    printf("\n");

    //TODO: Mesclar as partes particionadas e organizar o array por inteiro

    return 0;
}