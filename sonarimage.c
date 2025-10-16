#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_MATRIX_SIZE 2
#define MAX_MATRIX_SIZE 10
#define MAX_RANDOM_VALUE 256

// Function prototypes
void generateMatrix(int** matrixPtr, int size);
void displayMatrix(int* const* matrixPtr, int size);
void rotateMatrix(int** matrixPtr, int size);
void applySmoothingFilter(int** matrixPtr, int size);
void freeMatrix(int** matrixPtr, int size);

void generateMatrix(int** matrixPtr, int size)
{
    if (matrixPtr == NULL || size <= 0)
    {
        return;
    }

    for (int i = 0; i < size; ++i)
    {
        int* rowPtr = matrixPtr[i];
        for (int j = 0; j < size; ++j)
        {
            rowPtr[j] = rand() % MAX_RANDOM_VALUE;
        }
    }
}

void displayMatrix(int* const* matrixPtr, int size)
{
    if (matrixPtr == NULL || size <= 0)
    {
        return;
    }

    for (int i = 0; i < size; ++i)
    {
        const int* rowPtr = matrixPtr[i];
        for (int j = 0; j < size; ++j)
        {
            printf("%3d ", rowPtr[j]);
        }
        printf("\n");
    }
}

void rotateMatrix(int** matrixPtr, int size)
{
    if (matrixPtr == NULL || size <= 0)
    {
        return;
    }

    // Transpose
    for (int i = 0; i < size; ++i)
    {
        for (int j = i + 1; j < size; ++j)
        {
            int temp = matrixPtr[i][j];
            matrixPtr[i][j] = matrixPtr[j][i];
            matrixPtr[j][i] = temp;
        }
    }

    // Reverse rows
    for (int i = 0; i < size / 2; ++i)
    {
        int* temp = matrixPtr[i];
        matrixPtr[i] = matrixPtr[size - 1 - i];
        matrixPtr[size - 1 - i] = temp;
    }
}

void applySmoothingFilter(int** matrixPtr, int size)
{
    if (matrixPtr == NULL || size <= 0)
    {
        return;
    }

    int* tempRow = malloc(size * sizeof(int));
    int* prevRow = malloc(size * sizeof(int));

    if (tempRow == NULL || prevRow == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed in smoothing filter.\n");
        free(tempRow);
        free(prevRow);
        return;
    }

    for (int i = 0; i < size; ++i)
    {
        int* currentRow = matrixPtr[i];
        int* nextRow = (i < size - 1) ? matrixPtr[i + 1] : NULL;

        // Backup current row
        for (int j = 0; j < size; ++j)
        {
            tempRow[j] = currentRow[j];
        }

        for (int j = 0; j < size; ++j)
        {
            int sum = 0;
            int count = 0;

            for (int di = -1; di <= 1; ++di)
            {
                for (int dj = -1; dj <= 1; ++dj)
                {
                    int ni = i + di;
                    int nj = j + dj;

                    if (ni >= 0 && ni < size && nj >= 0 && nj < size)
                    {
                        int value = (ni == i - 1) ? prevRow[nj] :
                                    (ni == i)     ? tempRow[nj] :
                                                    nextRow[nj];

                        sum += value;
                        ++count;
                    }
                }
            }

            currentRow[j] = sum / count;
        }

        for (int j = 0; j < size; ++j)
        {
            prevRow[j] = tempRow[j];
        }
    }

    free(tempRow);
    free(prevRow);
}

void freeMatrix(int** matrixPtr, int size)
{
    if (matrixPtr == NULL)
    {
        return;
    }

    for (int i = 0; i < size; ++i)
    {
        free(matrixPtr[i]);
        matrixPtr[i] = NULL;
    }

    free(matrixPtr);
}

int main(void)
{
    int size = 0;

    printf("Enter matrix size (%d-%d): ", MIN_MATRIX_SIZE, MAX_MATRIX_SIZE);
    if (scanf("%d", &size) != 1)
    {
        fprintf(stderr, "Error: Invalid input.\n");
        return EXIT_FAILURE;
    }

    if (size < MIN_MATRIX_SIZE || size > MAX_MATRIX_SIZE)
    {
        fprintf(stderr, "Error: Matrix size must be between %d and %d.\n",
                MIN_MATRIX_SIZE, MAX_MATRIX_SIZE);
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));

    int** matrixPtr = malloc(size * sizeof(int*));
    if (matrixPtr == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed for matrix.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < size; ++i)
    {
        matrixPtr[i] = malloc(size * sizeof(int));
        if (matrixPtr[i] == NULL)
        {
            fprintf(stderr, "Error: Memory allocation failed for row %d.\n", i);
            freeMatrix(matrixPtr, i);
            return EXIT_FAILURE;
        }
    }

    generateMatrix(matrixPtr, size);
    printf("\nOriginal Matrix:\n");
    displayMatrix(matrixPtr, size);

    rotateMatrix(matrixPtr, size);
    printf("\nMatrix After 90-Degree Rotation:\n");
    displayMatrix(matrixPtr, size);

    applySmoothingFilter(matrixPtr, size);
    printf("\nMatrix After 3x3 Smoothing Filter:\n");
    displayMatrix(matrixPtr, size);

    freeMatrix(matrixPtr, size);
    return EXIT_SUCCESS;
}