#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

char *createSubArray(char original[], int size)
{
    char *subArray = (char *)malloc(size * sizeof(char));
    if (subArray == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < size; i++)
    {
        subArray[i] = original[i];
    }

    return subArray;
}

void readFromFile(char arr[])
{
    FILE *file;
    long file_length;

    // Open the file
    file = fopen("input.txt", "r");

    if (file == NULL)
    {
        perror("Error opening file");
    }

    if (arr == NULL)
    {
        perror("Error allocating memory");
        fclose(file);
    }

    char line[500];
    // Read file contents into buffer
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Concatenate the line to buffer
        strcat(arr, line);
    }
    // Close the file
    fclose(file);
}

void encrypt(char message[])
{
    size_t length = strlen(message);
    for (int i = 0; i < length; i++)
    {
        // printf("%c ", message[i]);
        message[i] = message[i] + 3;
    }
}

void decrypt(char message[])
{
    size_t length = strlen(message);
    for (int i = 0; i < length; i++)
    {
        message[i] = message[i] - 3;
    }
}

int main(int argc, char *argv[])
{
    int procesRank;    /* rank of process */
    int p;             /* number of process */
    int src;           /* rank of sender */
    int dest;          /* rank of reciever */
    int tag = 0;       /* tag for messages */
    char message[500]; /* storage for message */
    MPI_Status status; /* return status for recieve */
    int mode, algo;
    int limit;

    /* Start up MPI */
    MPI_Init(&argc, &argv);

    /* Find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &procesRank);

    /* Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (procesRank == 0)
    {
        // get algorithm mode and validate it
        printf("\nChoose algorithm mode\n 1. Encryption. \n 2. Decryption.\n");
        scanf("%d", &algo);

        while (algo != 1 && algo != 2)
        {
            printf("Incorrect input, please try again: ");
            scanf("%d", &algo);
        }

        // get read mode and validate it
        printf("\nChoose read mode then enter your text:\n 1. Read from input. \n 2. Read from file.\n");
        scanf("%d", &mode);

        while (mode != 1 && mode != 2)
        {
            printf("Incorrect input, please try again: ");
            scanf("%d", &mode);
        }

        // Either read from console or read from file
        if (mode == 1)
        {
            scanf("\n");
            fgets(message, 500, stdin);
        }
        else
        {
            readFromFile(message);
        }

        // divide the array into equal portions
        int totalLength = strlen(message);
        if(message[totalLength] == '\n') totalLength--;
        int sz = totalLength / (p - 1);
        int i = 0;
        limit = p < totalLength ? p : totalLength;

        for (int destnation = 1; destnation < limit; destnation++)
        {
            char *subArr = createSubArray(message + (sz * i), sz);
            MPI_Send(&algo, 1, MPI_INT, destnation, 0, MPI_COMM_WORLD);
            MPI_Send(&sz, 1, MPI_INT, destnation, 0, MPI_COMM_WORLD);
            MPI_Send(subArr, sz, MPI_CHAR, destnation, 0, MPI_COMM_WORLD);
            i++;
        }

        char result[500];
        char temp[500];
        for (int source = 1; source < limit; source++)
        {
            MPI_Recv(temp, sz, MPI_CHAR, source, 0, MPI_COMM_WORLD, &status);
            temp[sz] = '\0';
            strcat(result, temp);
        }

        printf("From master process the result is: %s \n", result);
    }
    else if(procesRank < limit - 1)
    {
        int sz;
        MPI_Recv(&algo, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&sz, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(message, sz, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        if (algo == 1)
        {
            encrypt(message);
        }
        else
        {
            decrypt(message);
        }
        MPI_Send(message, sz, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    /* shutdown MPI */
    MPI_Finalize();

    return 0;
}
