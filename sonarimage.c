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

    for (int rowIndex = 0; rowIndex < size; ++rowIndex)
    {
        int* rowPtr = matrixPtr[rowIndex];
        for (int colIndex = 0; colIndex < size; ++colIndex)
        {
            rowPtr[colIndex] = rand() % MAX_RANDOM_VALUE;
        }
    }
}

void displayMatrix(int* const* matrixPtr, int size)
{
    if (matrixPtr == NULL || size <= 0)
    {
        return;
    }

    for (int rowIndex = 0; rowIndex < size; ++rowIndex)
    {
        const int* rowPtr = matrixPtr[rowIndex];
        for (int colIndex = 0; colIndex < size; ++colIndex)
        {
            printf("%3d ", rowPtr[colIndex]);
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
    for (int rowIndex = 0; rowIndex < size; ++rowIndex)
    {
        for (int colIndex = rowIndex + 1; colIndex < size; ++colIndex)
        {
            int tempValue = matrixPtr[rowIndex][colIndex];
            matrixPtr[rowIndex][colIndex] = matrixPtr[colIndex][rowIndex];
            matrixPtr[colIndex][rowIndex] = tempValue;
        }
    }

    // Reverse rows
    for (int rowIndex = 0; rowIndex < size / 2; ++rowIndex)
    {
        int* tempRowPtr = matrixPtr[rowIndex];
        matrixPtr[rowIndex] = matrixPtr[size - 1 - rowIndex];
        matrixPtr[size - 1 - rowIndex] = tempRowPtr;
    }
}

void applySmoothingFilter(int** matrixPtr, int size)
{
    if (matrixPtr == NULL || size <= 0)
    {
        return;
    }

    int* previousRowPtr = malloc(size * sizeof(int));
    int* backupRowPtr = malloc(size * sizeof(int));

    if (previousRowPtr == NULL || backupRowPtr == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed in smoothing filter.\n");
        free(previousRowPtr);
        free(backupRowPtr);
        return;
    }

    for (int rowIndex = 0; rowIndex < size; ++rowIndex)
    {
        int* currentRowPtr = matrixPtr[rowIndex];
        int* nextRowPtr = (rowIndex < size - 1) ? matrixPtr[rowIndex + 1] : NULL;

        // Backup current row
        for (int colIndex = 0; colIndex < size; ++colIndex)
        {
            backupRowPtr[colIndex] = currentRowPtr[colIndex];
        }

        for (int colIndex = 0; colIndex < size; ++colIndex)
        {
            int sumValue = 0;
            int neighborCount = 0;

            for (int rowOffset = -1; rowOffset <= 1; ++rowOffset)
            {
                for (int colOffset = -1; colOffset <= 1; ++colOffset)
                {
                    int neighborRowIndex = rowIndex + rowOffset;
                    int neighborColIndex = colIndex + colOffset;

                    if (neighborRowIndex >= 0 && neighborRowIndex < size &&
                        neighborColIndex >= 0 && neighborColIndex < size)
                    {
                        int neighborValue = (neighborRowIndex == rowIndex - 1) ? previousRowPtr[neighborColIndex] :
                                            (neighborRowIndex == rowIndex)     ? backupRowPtr[neighborColIndex] :
                                                                                nextRowPtr[neighborColIndex];

                        sumValue += neighborValue;
                        ++neighborCount;
                    }
                }
            }

            currentRowPtr[colIndex] = sumValue / neighborCount;
        }

        for (int colIndex = 0; colIndex < size; ++colIndex)
        {
            previousRowPtr[colIndex] = backupRowPtr[colIndex];
        }
    }

    free(previousRowPtr);
    free(backupRowPtr);
}

void freeMatrix(int** matrixPtr, int size)
{
    if (matrixPtr == NULL)
    {
        return;
    }

    for (int rowIndex = 0; rowIndex < size; ++rowIndex)
    {
        free(matrixPtr[rowIndex]);
        matrixPtr[rowIndex] = NULL;
    }

    free(matrixPtr);
}

int main(void)
{
    int matrixSize = 0;

    printf("Enter matrix size (%d-%d): ", MIN_MATRIX_SIZE, MAX_MATRIX_SIZE);
    if (scanf("%d", &matrixSize) != 1)
    {
        fprintf(stderr, "Error: Invalid input.\n");
        return EXIT_FAILURE;
    }

    if (matrixSize < MIN_MATRIX_SIZE || matrixSize > MAX_MATRIX_SIZE)
    {
        fprintf(stderr, "Error: Matrix size must be between %d and %d.\n",
                MIN_MATRIX_SIZE, MAX_MATRIX_SIZE);
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));

    int** matrixPtr = malloc(matrixSize * sizeof(int*));
    if (matrixPtr == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed for matrix.\n");
        return EXIT_FAILURE;
    }

    for (int rowIndex = 0; rowIndex < matrixSize; ++rowIndex)
    {
        matrixPtr[rowIndex] = malloc(matrixSize * sizeof(int));
        if (matrixPtr[rowIndex] == NULL)
        {
            fprintf(stderr, "Error: Memory allocation failed for row %d.\n", rowIndex);
            freeMatrix(matrixPtr, rowIndex);
            return EXIT_FAILURE;
        }
    }

    generateMatrix(matrixPtr, matrixSize);
    printf("\nOriginal Matrix:\n");
    displayMatrix(matrixPtr, matrixSize);

    rotateMatrix(matrixPtr, matrixSize);
    printf("\nMatrix After 90-Degree Rotation:\n");
    displayMatrix(matrixPtr, matrixSize);

    applySmoothingFilter(matrixPtr, matrixSize);
    printf("\nMatrix After 3x3 Smoothing Filter:\n");
    displayMatrix(matrixPtr, matrixSize);

    freeMatrix(matrixPtr, matrixSize);
    return EXIT_SUCCESS;
}