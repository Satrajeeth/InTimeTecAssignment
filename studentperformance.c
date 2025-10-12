#include <stdio.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_STUDENTS 100
#define SUBJECT_COUNT 3

typedef struct StudentRecord
{
    int rollNumber;
    char name[MAX_NAME_LENGTH];
    int marks[SUBJECT_COUNT];
    int total;
    float average;
    char grade;
} StudentRecord;

int gStudentCount = 0;

float calculateAverage(int total)
{
    return (float)total / SUBJECT_COUNT;
}

int calculateTotal(int marks1, int marks2, int marks3)
{
    return marks1 + marks2 + marks3;
}

char assignGrade(float average)
{
    if (average >= 85)
    {
        return 'A';
    }
    else if (average >= 70)
    {
        return 'B';
    }
    else if (average >= 50)
    {
        return 'C';
    }
    else if (average >= 35)
    {
        return 'D';
    }
    else
    {
        return 'F';
    }
}

void printStars(char grade, FILE* outputFile)
{
    int starCount = 0;
    
    if (grade == 'A')
    {
        starCount = 5;
    }
    else if (grade == 'B')
    {
        starCount = 4;
    }
    else if (grade == 'C')
    {
        starCount = 3;
    }
    else if (grade == 'D')
    {
        starCount = 2;
    }
    
    for (int i = 0; i < starCount; i++)
    {
        fprintf(outputFile, "*");
    }
}

void printRollNumbers(int current, int total, FILE* outputFile)
{
    if (current > total)
    {
        return;
    }
    
    fprintf(outputFile, "%d", current);
    
    if (current < total)
    {
        fprintf(outputFile, " ");
    }
    
    printRollNumbers(current + 1, total, outputFile);
}

void processStudent(StudentRecord* student, FILE* outputFile)
{
    student->total = calculateTotal(student->marks[0], 
                                    student->marks[1], 
                                    student->marks[2]);
    
    student->average = calculateAverage(student->total);
    
    student->grade = assignGrade(student->average);
    
    fprintf(outputFile, "Roll: %d\n", student->rollNumber);
    fprintf(outputFile, "Name: %s\n", student->name);
    fprintf(outputFile, "Total: %d\n", student->total);
    fprintf(outputFile, "Average: %.2f\n", student->average);
    fprintf(outputFile, "Grade: %c\n", student->grade);
    
    if (student->average < 35)
    {
        fprintf(outputFile, "\n");
        return;
    }
    
    fprintf(outputFile, "Performance: ");
    printStars(student->grade, outputFile);
    fprintf(outputFile, "\n\n");
}

int main()
{
    FILE* inputFile = fopen("input.txt", "r");
    FILE* outputFile = fopen("output.txt", "w");
    
    if (inputFile == NULL)
    {
        printf("Error: Could not open input.txt\n");
        return 1;
    }
    
    if (outputFile == NULL)
    {
        printf("Error: Could not open output.txt\n");
        fclose(inputFile);
        return 1;
    }
    
    StudentRecord students[MAX_STUDENTS];
    int numberOfStudents = 0;
    
    fscanf(inputFile, "%d", &numberOfStudents);
    gStudentCount = numberOfStudents;
    
    for (int i = 0; i < numberOfStudents; i++)
    {
        fscanf(inputFile, "%d %s %d %d %d", 
               &students[i].rollNumber,
               students[i].name,
               &students[i].marks[0],
               &students[i].marks[1],
               &students[i].marks[2]);
        
        processStudent(&students[i], outputFile);
    }
    
    fprintf(outputFile, "List of Roll Numbers: ");
    printRollNumbers(1, numberOfStudents, outputFile);
    fprintf(outputFile, "\n");
    
    fclose(inputFile);
    fclose(outputFile);
    
    printf("Processing complete. Check output.txt for results.\n");
    
    return 0;
}