#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

typedef unsigned int uint;

#define FILES_COUNT 10
#define MAX_FILENAME 30

// Pode mudar aqui a palavra que deseja procurar
// Mas ela devera estar com todas as letras minusculas
const char* c_SearchedWord = "amor";
uint searchWordSize = 0;

const char* c_Ignorable = " \t\n.,;:!?'\"\\/()[]{}<>-";
const uint c_IgnorableSize = 22;

uint wordCount = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/// @brief Procura pela palavra escolhida em c_SearchedWord independente
/// @brief Se no texto estiver maiuscula ou minuscula
/// @param filename 
/// @return 
void* FindWord(void* filename)
{
    FILE* file = fopen((char*) filename, "rt");

    if (file != NULL)
    {
        char currentChar = 0; 
        char previousChar = 0;
        unsigned int i = 0;

        while ((currentChar = fgetc(file)) != EOF)
        {
            currentChar = tolower(currentChar);

            // Checa se ja comparou todas as letras da palavra procurada
            if (i >= searchWordSize)
            {
                // Checando se o caracter apos a ultima letra da palavra
                // que esta dando match eh um dos caracteres ignoraveis
                int isIgnorable = 0;
                for (unsigned int j = 0; j < c_IgnorableSize && isIgnorable == 0; j++)
                {
                    if (currentChar == c_Ignorable[j])
                        isIgnorable = 1;
                }

                // Se o caracter apos a ultima letra encontrada nao for ignoravel
                // entao a palavra encontrada nao eh uma palavra por si so
                // mas eh na realidade uma palavra que contem outra palavra
                // como a palavra "couve" que contem a palavra "ou"
                if (isIgnorable)
                {
                    pthread_mutex_lock(&mutex);

                    wordCount++;

                    pthread_mutex_unlock(&mutex);
                }
                
                i = 0;
            }

            // Checando letra da palavra com a letra do texto
            if (currentChar == c_SearchedWord[i])
            {
                i++;

                // Se o i for 1 entao antes ela era 0 ou seja esse foi o primeiro
                // caracter a dar match
                if (i == 1)
                {
                    // Considere a palavra procurada seja "ou"
                    // Precisamos entao checar se antes desse primeiro caracter havia um
                    // Caracter ignoravel como no caso "(ou" onde o parenteses eh ignoravel
                    // Mas no caso "couve" que contem a palavra "ou" o 'c' nao eh ignoravel
                    int isIgnorable = 0;
                    for (unsigned int j = 0; j < c_IgnorableSize && isIgnorable == 0; j++)
                    {
                        if(previousChar == c_Ignorable[j])
                            isIgnorable = 1;
                    }

                    if (!isIgnorable)
                        i = 0;
                }
            }
            else
                i = 0; // Se forem diferentes voltamos para a primeira letra da palavra procurada

            previousChar = currentChar;
        }

        fclose(file);
    }
    else
        printf("Thread failed to open file %s\n", (char*)filename);
}

int main()
{
    char filenames[FILES_COUNT][MAX_FILENAME] = 
    {
        "text_0.txt",
        "text_1.txt",
        "text_2.txt",
        "text_3.txt",
        "text_4.txt",
        "text_5.txt",
        "text_6.txt",
        "text_7.txt",
        "text_8.txt",
        "text_9.txt"
    };
    pthread_t threads[FILES_COUNT];

    // Calculando dinamicamente o tamanho da palavra procurada
    searchWordSize = strlen(c_SearchedWord);

    for (unsigned int i = 0; i < FILES_COUNT; i++)
        pthread_create(&threads[i], NULL, FindWord, filenames[i]);

    for (unsigned int i = 0; i < FILES_COUNT; i++)
        pthread_join(threads[i], NULL);

    printf("Word Count: %d\n", wordCount);
    return 0;
}
