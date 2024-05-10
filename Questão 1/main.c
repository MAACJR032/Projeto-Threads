#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#define FILES_COUNT 10
#define MAX_FILENAME 30

const char* c_SearchedWord = "palavra";
unsigned int wordCount = 0;

pthread_mutex_t mutex;

void* FindWord(void* filename)
{
    FILE* file = fopen((char *) filename, "rt");

    if (file != NULL)
    {
        const char* ignorable = " \t\n.,;:!?'\"\\/()[]{}<>-";
        char currentChar = 0;
        char previousChar = 0;
        unsigned int i = 0;
        unsigned int searchWordSize = strlen(c_SearchedWord);
        unsigned int ignorableSize = strlen(ignorable);

        while ((currentChar = fgetc(file)) != EOF)
        {
            currentChar = tolower(currentChar);

            if (i >= searchWordSize)
            {
                // Checando se o caracter após a ultima letra da palavra
                // que esta dando match é um dos caracteres ignoraveis
                int isIgnorable = 0;
                for (unsigned int j = 0; j < ignorableSize && isIgnorable == 0; j++)
                {
                    if (currentChar == ignorable[j])
                        isIgnorable = 1;
                }

                /*
                 * Se o caracter apos a ultima letra encontrada nao for ignoravel
                 * entao a palavra encontrada nao eh uma palavra por si so
                 * mas eh na realidade uma palavra que contem outra palavra
                 * como a palavra "couve" que contem a palavra "ou"
                 */
                if (isIgnorable)
                {
                    pthread_mutex_lock(&mutex);
                    wordCount++;
                    pthread_mutex_unlock(&mutex);

                    printf("Encontrada a palavra no arquivo %s\n", (char*)filename);
                }
                
                i = 0;
            }

            if (currentChar == c_SearchedWord[i])
            {
                i++;

                // Se o i for um entao antes ela era 0 ou seja esse foi o primeiro
                // caracter a dar match
                if (i == 1)
                {
                    int isIgnorable = 0;
                    for(unsigned int j = 0; j < ignorableSize && isIgnorable == 0; j++)
                    {
                        if (previousChar == ignorable[j])
                            isIgnorable = 1;
                    }

                    if (!isIgnorable)
                        i = 0;
                }
            }
            else
                i = 0;

            previousChar = currentChar;
        }

        fclose(file);
    }
    else
        printf("Thread failed to open file %s\n", (char *) filename);
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
    pthread_mutex_init(&mutex, NULL);

    for (unsigned int i = 0; i < FILES_COUNT; i++)
        pthread_create(&threads[i], NULL, FindWord, filenames[i]);

    for (unsigned int i = 0; i < FILES_COUNT; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&mutex);
    printf("Word Count: %d\n", wordCount);
    return 0;
}