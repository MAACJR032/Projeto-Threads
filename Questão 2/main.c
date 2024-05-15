#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>

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
   printf("This thread arr size: %d\n", arrayLimits->Size);
}

int main()
{
    srand(0);
    //int arr[MIN_ARRAY_SIZE] = { 9, 5, 1, 3, 7, 12, 6, 96, 5, 9874, 56, 515, 12, 15, 17, 654, 98, 84, 47, 46 };
    int arr[101];
    for(unsigned int i = 0; i < 101; i++)
    {
        arr[i] = rand() % 101;
        printf("%d ", arr[i]);
    }
    printf("\n");

    int arrSize = sizeof(arr) / sizeof(int);

    BubbleSortArgs args[MAX_THREADS];
    pthread_t threads[MAX_THREADS];
    int indexes[MAX_THREADS];

    //Inicializando a primeira thread
    args[0].ArrayBegin = arr;
    args[0].Size = (arrSize / MAX_THREADS) + (arrSize % MAX_THREADS);
    indexes[0] = 0;
    pthread_create(&threads[0], NULL, BubbleSort, &args[0]);
    
    for (unsigned int i = 1, offset = args[0].Size; i < MAX_THREADS; i++)
    {
        args[i].ArrayBegin = arr + offset;
        args[i].Size = arrSize / MAX_THREADS;
        offset += args[i].Size;
        indexes[i] = 0;
        
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
    int newArr[arrSize];
    for (unsigned int i = 0; i < arrSize; i++)
    {
        int min = INT_MAX;
        int subArrayPicked = 0;

        for (unsigned int j = 0; j < MAX_THREADS; j++)
        {
            if(indexes[j] >= args[j].Size)
                continue;

            int n = args[j].ArrayBegin[indexes[j]];
            if(n < min)
            {
                min = n;
                subArrayPicked = j;
            }
        }

        indexes[subArrayPicked]++;
        newArr[i] = min;
    }

    for (unsigned int i = 0; i < arrSize; i++)
        printf("%d ", newArr[i]);
    printf("\n");

    return 0;
}