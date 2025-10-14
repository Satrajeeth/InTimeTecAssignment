#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateMatrix(int** matrix, int size)
{
    if (matrix == NULL || size <= 0)
    {
        return;
    }
    
    for (int i = 0; i < size; ++i)
    {
        int* rowPtr = *(matrix + i);
        for (int j = 0; j < size; ++j)
        {
            *(rowPtr + j) = rand() % 256;
        }
    }
}

void displayMatrix(int** matrix, int size)
{
    if (matrix == NULL || size <= 0)
    {
        return;
    }
    
    for (int i = 0; i < size; ++i)
    {
        int* rowPtr = *(matrix + i);
        for (int j = 0; j < size; ++j)
        {
            printf("%3d ", *(rowPtr + j));
        }
        printf("\n");
    }
}

void rotateMatrix(int** matrix, int size)
{
    if (matrix == NULL || size <= 0)
    {
        return;
    }
    
    for (int i = 0; i < size; ++i)
    {
        for (int j = i + 1; j < size; ++j)
        {
            int* rowI = *(matrix + i);
            int* rowJ = *(matrix + j);
            
            int temp = *(rowI + j);
            *(rowI + j) = *(rowJ + i);
            *(rowJ + i) = temp;
        }
    }
    
    for (int i = 0; i < size / 2; ++i)
    {
        int* temp = *(matrix + i);
        *(matrix + i) = *(matrix + (size - 1 - i));
        *(matrix + (size - 1 - i)) = temp;
    }
}

void applySmoothingFilter(int** matrix, int size)
{
    if (matrix == NULL || size <= 0)
    {
        return;
    }
    
    int* tempRow = (int*)malloc(size * sizeof(int));
    if (tempRow == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    
    int* prevRow = (int*)malloc(size * sizeof(int));
    if (prevRow == NULL)
    {
        free(tempRow);
        return;
    }
    
    for (int i = 0; i < size; ++i)
    {
        int* currentRow = *(matrix + i);
        int* nextRow = (i < size - 1) ? *(matrix + i + 1) : NULL;
        
        for (int j = 0; j < size; ++j)
        {
            *(tempRow + j) = *(currentRow + j);
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
                        int value;
                        
                        if (ni == i - 1)
                        {
                            value = *(prevRow + nj);
                        }
                        else if (ni == i)
                        {
                            value = *(tempRow + nj);
                        }
                        else
                        {
                            value = *(nextRow + nj);
                        }
                        
                        sum += value;
                        ++count;
                    }
                }
            }
            
            *(currentRow + j) = sum / count;
        }
        
        for (int j = 0; j < size; ++j)
        {
            *(prevRow + j) = *(tempRow + j);
        }
    }
    
    free(tempRow);
    free(prevRow);
}

int main(void)
{
    int size = 0;
    
    printf("Enter matrix size (2-10): ");
    if (scanf("%d", &size) != 1)
    {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }
    
    if (size < 2 || size > 10)
    {
        fprintf(stderr, "Matrix size must be between 2 and 10\n");
        return 1;
    }
    
    srand((unsigned int)time(NULL));
    
    int** matrix = (int**)malloc(size * sizeof(int*));
    if (matrix == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    for (int i = 0; i < size; ++i)
    {
        *(matrix + i) = (int*)malloc(size * sizeof(int));
        if (*(matrix + i) == NULL)
        {
            for (int j = 0; j < i; ++j)
            {
                free(*(matrix + j));
            }
            free(matrix);
            return 1;
        }
    }
    
    generateMatrix(matrix, size);
    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrix, size);
    
    rotateMatrix(matrix, size);
    printf("\nMatrix after 90 Degree Clockwise Rotation:\n");
    displayMatrix(matrix, size);
    
    applySmoothingFilter(matrix, size);
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrix, size);
    
    for (int i = 0; i < size; ++i)
    {
        free(*(matrix + i));
        *(matrix + i) = NULL;
    }
    free(matrix);
    matrix = NULL;
    
    return 0;
}