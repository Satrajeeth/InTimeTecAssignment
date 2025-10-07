#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BUFFER_SIZE 100

//Remove White spaces
void removeWhitespace(char* str)
{
    int readIndex = 0;
    int writeIndex = 0;

    while (str[readIndex])
    {
        if (!isspace((unsigned char)str[readIndex]))
        {
            str[writeIndex++] = str[readIndex];
        }
        readIndex++;
    }
    str[writeIndex] = '\0';
}

bool isValidExpression(const char* str)
{
    for (int i = 0; str[i]; i++)
    {
        if (!(isdigit((unsigned char)str[i]) || str[i] == '+' || str[i] == '-' ||
              str[i] == '*' || str[i] == '/'))
        {
            return false;
        }
    }
    return true;
}

int calculateExpression(char* expression, int* errorCode)
{
    int numbers[MAX_BUFFER_SIZE];
    int operators[MAX_BUFFER_SIZE];
    int numbersCount = 0;
    int operatorsCount = 0;
    int index = 0;

    while (expression[index])
    {
        if (isdigit((unsigned char)expression[index]))
        {
            int value = 0;
            while (isdigit((unsigned char)expression[index]))
            {
                value = value * 10 + (expression[index] - '0');
                index++;
            }
            numbers[numbersCount++] = value;
        }
        else
        {
            if (expression[index] == '+' || expression[index] == '-' ||
                expression[index] == '*' || expression[index] == '/')
            {
                operators[operatorsCount++] = expression[index];
                index++;
            }
            else
            {
                index++;
            }
        }
    }

    for (int j = 0; j < operatorsCount;)
    {
        if (operators[j] == '*' || operators[j] == '/')
        {
            if (operators[j] == '*')
            {
                numbers[j] = numbers[j] * numbers[j + 1];
            }
            else
            {
                if (numbers[j + 1] == 0)
                {
                    *errorCode = 2;
                    return 0;
                }
                numbers[j] = numbers[j] / numbers[j + 1];
            }

            for (int k = j + 1; k < numbersCount - 1; k++)
            {
                numbers[k] = numbers[k + 1];
            }
            for (int k = j; k < operatorsCount - 1; k++)
            {
                operators[k] = operators[k + 1];
            }
            numbersCount--;
            operatorsCount--;
        }
        else
        {
            j++;
        }
    }

    int result = numbers[0];
    for (int j = 0; j < operatorsCount; j++)
    {
        if (operators[j] == '+')
        {
            result += numbers[j + 1];
        }
        else
        {
            result -= numbers[j + 1];
        }
    }

    return result;
}

int main(void)
{
    char inputBuffer[MAX_BUFFER_SIZE];
    if (!fgets(inputBuffer, sizeof(inputBuffer), stdin))
    {
        return 0;
    }

    inputBuffer[strcspn(inputBuffer, "\n")] = 0;

    removeWhitespace(inputBuffer);

    if (!isValidExpression(inputBuffer) || strlen(inputBuffer) == 0)
    {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    int errorCode = 0;
    int result = calculateExpression(inputBuffer, &errorCode);

    if (errorCode == 2)
    {
        printf("Error: Division by zero.\n");
        return 0;
    }

    printf("%d\n", result);
    return 0;
}