#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int main(void)
{
    int numberInput = 0;
    int digitSum = 0;
    
    scanf("%d", &numberInput);
    
    while (numberInput)
    {
        digitSum += numberInput % 10;
        numberInput /= 10;
    }
    
    printf("%d\n", digitSum);
    return 0;
}