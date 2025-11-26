#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();
char* ltrim(char*);
char* rtrim(char*);
int parse_int(char*);

int stringCompare(char* str1, char* str2)
{
    char* ptr1 = str1;
    char* ptr2 = str2;
    
    while (*ptr1 != '\0' && *ptr2 != '\0')
    {
        if (*ptr1 != *ptr2)
        {
            return 0;
        }
        ptr1++;
        ptr2++;
    }
    
    if (*ptr1 == '\0' && *ptr2 == '\0')
    {
        return 1;
    }
    
    return 0;
}

int* matchingStrings(int stringList_count, char** stringList, int queries_count, char** queries, int* result_count)
{
    *result_count = queries_count;
    
    int* result = malloc(queries_count * sizeof(int));
    
    int* resultPtr = result;
    char** queryPtr = queries;
    
    int i;
    int j;
    
    for (i = 0; i < queries_count; i++)
    {
        int count = 0;
        char** listPtr = stringList;
        
        for (j = 0; j < stringList_count; j++)
        {
            if (stringCompare(*queryPtr, *listPtr))
            {
                count++;
            }
            listPtr++;
        }
        
        *resultPtr = count;
        resultPtr++;
        queryPtr++;
    }
    
    return result;
}

int main()
{
    FILE* fptr = fopen(getenv("OUTPUT_PATH"), "w");
    
    int stringList_count = parse_int(ltrim(rtrim(readline())));
    
    char** stringList = malloc(stringList_count * sizeof(char*));
    
    for (int i = 0; i < stringList_count; i++) {
        char* stringList_item = readline();
        *(stringList + i) = stringList_item;
    }
    
    int queries_count = parse_int(ltrim(rtrim(readline())));
    
    char** queries = malloc(queries_count * sizeof(char*));
    
    for (int i = 0; i < queries_count; i++) {
        char* queries_item = readline();
        *(queries + i) = queries_item;
    }
    
    int res_count;
    int* res = matchingStrings(stringList_count, stringList, queries_count, queries, &res_count);
    
    for (int i = 0; i < res_count; i++) {
        fprintf(fptr, "%d", *(res + i));
        
        if (i != res_count - 1) {
            fprintf(fptr, "\n");
        }
    }
    
    fprintf(fptr, "\n");
    
    fclose(fptr);
    
    return 0;
}

char* readline() {
    size_t alloc_length = 1024;
    size_t data_length = 0;
    
    char* data = malloc(alloc_length);
    
    while (true) {
        char* cursor = data + data_length;
        char* line = fgets(cursor, alloc_length - data_length, stdin);
        
        if (!line) {
            break;
        }
        
        data_length += strlen(cursor);
        
        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n') {
            break;
        }
        
        alloc_length <<= 1;
        
        data = realloc(data, alloc_length);
        
        if (!data) {
            data = NULL;
            break;
        }
    }
    
    if (data[data_length - 1] == '\n') {
        data[data_length - 1] = '\0';
        
        data = realloc(data, data_length);
        
        if (!data) {
            data = NULL;
        }
    } else {
        data = realloc(data, data_length + 1);
        
        if (!data) {
            data = NULL;
        } else {
            data[data_length] = '\0';
        }
    }
    
    return data;
}

char* ltrim(char* str) {
    if (!str) {
        return NULL;
    }
    
    if (!*str) {
        return str;
    }
    
    while (*str != '\0' && isspace(*str)) {
        str++;
    }
    
    return str;
}

char* rtrim(char* str) {
    if (!str) {
        return NULL;
    }
    
    if (!*str) {
        return str;
    }
    
    char* end = str + strlen(str) - 1;
    
    while (end >= str && isspace(*end)) {
        end--;
    }
    
    *(end + 1) = '\0';
    
    return str;
}

int parse_int(char* str) {
    char* endptr;
    int value = strtol(str, &endptr, 10);
    
    if (endptr == str || *endptr != '\0') {
        exit(EXIT_FAILURE);
    }
    
    return value;
}