#define _XOPEN_SOURCE 600

#include <limits.h>
#include <memory.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREADS 5
#define MIN_ARRAY_SIZE 20

// Os argumentos para o bubble sort são o tamanho do array
// e o endereço do primeiro elemento do array
typedef struct {
  int Size, *ArrayBegin;
} BubbleSortArgs;

typedef struct {
  int *Arr, ArrSize, NumThreads;
  BubbleSortArgs *Args;
} MergeSubArraysArgs;

pthread_barrier_t mergeBarrier;

void Swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void *BubbleSort(void *args) {
  BubbleSortArgs *arrayLimits = (BubbleSortArgs *)args;

  for (int i = 0; i < arrayLimits->Size; i++) {
    for (int j = 0; j < arrayLimits->Size - i - 1; j++) {
      if (arrayLimits->ArrayBegin[j] > arrayLimits->ArrayBegin[j + 1])
        Swap(&arrayLimits->ArrayBegin[j], &arrayLimits->ArrayBegin[j + 1]);
    }
  }

  pthread_barrier_wait(&mergeBarrier);

  return NULL;
}

/// @brief Mesclar os subarrays organizado
/// @param arrSize
/// @param args
/// @param numThreads
void *MergeSubArrays(void *args) {
  MergeSubArraysArgs *mergeArrayData = (MergeSubArraysArgs *)args;

  int newArr[mergeArrayData->ArrSize];
  int indexes[mergeArrayData->NumThreads];
  // Zera os indices
  memset(indexes, 0, sizeof(indexes));

  pthread_barrier_wait(&mergeBarrier);

  // Se os sub arrays forem por exemplo
  // 1 5 7 9 - Indice 0
  // 2 3 4 6 - Indice 0
  // - - - - - - - - Array Total
  // Pegamos o menor elemento entre os dois menores elementos desse array
  // Ou seja comparamos quem é menor 1 ou 2, como o 1 é menor pegamos o 1
  // e avançamos no subarray escolhido
  // - 5 7 9 - Indice 1
  // 2 3 4 6 - Indice 0
  // 1 - - - - - - - Array total
  // Agora comparamos 5 com 2 e pegamos o menor e assim por diante
  for (unsigned int i = 0; i < mergeArrayData->ArrSize; i++) {
    int min = INT_MAX;
    int subArrayPicked = 0;

    // Escolhe o menor valor entre os subarrays
    for (unsigned int j = 0; j < mergeArrayData->NumThreads; j++) {
      if (indexes[j] >= mergeArrayData->Args[j].Size)
        continue;

      int n = mergeArrayData->Args[j].ArrayBegin[indexes[j]];
      if (n < min) {
        min = n;
        subArrayPicked = j;
      }
    }

    indexes[subArrayPicked]++;
    newArr[i] = min;
  }

  // Copia o array organizado para o array original
  memcpy(mergeArrayData->Arr, newArr, sizeof(newArr));

  return NULL;
}

int main() {
  int arr[MIN_ARRAY_SIZE] = {9,  5,   1,  3,  7,  12,  6,  96, 5,  9874,
                             56, 515, 12, 15, 17, 654, 98, 84, 47, 46};

  int arrSize = sizeof(arr) / sizeof(int);

  BubbleSortArgs args[MAX_THREADS];
  pthread_t threads[MAX_THREADS];
  pthread_t mergeThread;
  MergeSubArraysArgs mergeArg;

  // Cria a barreira que sincroniza todas as threads com os sub arrays +
  // A thread que mescla os sub arrays para assim formar o array total ordenado
  pthread_barrier_init(&mergeBarrier, NULL, MAX_THREADS + 1);

  // Inicializando a primeira thread
  args[0].ArrayBegin = arr;
  args[0].Size = (arrSize / MAX_THREADS) + (arrSize % MAX_THREADS);
  pthread_create(&threads[0], NULL, BubbleSort, &args[0]);

  for (unsigned int i = 1, offset = args[0].Size; i < MAX_THREADS; i++) {
    args[i].ArrayBegin = arr + offset;
    args[i].Size = arrSize / MAX_THREADS;
    offset += args[i].Size;

    pthread_create(&threads[i], NULL, BubbleSort, &args[i]);
  }

  mergeArg.Arr = arr;
  mergeArg.ArrSize = arrSize;
  mergeArg.NumThreads = MAX_THREADS;
  mergeArg.Args = args;

  // Criando a thread que mescla os sub arrays
  pthread_create(&mergeThread, NULL, MergeSubArrays, &mergeArg);

  // Esperando a thread que mescla os sub arrays terminarem
  pthread_join(mergeThread, NULL);

  pthread_barrier_destroy(&mergeBarrier);

  for (unsigned int i = 0; i < arrSize; i++)
    printf("%d ", arr[i]);
  printf("\n");

  return 0;
}
