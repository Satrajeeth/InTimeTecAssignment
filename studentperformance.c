#include <stdio.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_STUDENTS 100
#define SUBJECT_COUNT 3

typedef enum { A = 'A', B = 'B', C = 'C', D = 'D', F = 'F' } Grade;

typedef struct StudentRecord
{
    int rollNumber;
    char name[MAX_NAME_LENGTH];
    int marks[SUBJECT_COUNT];
    int total;
    float average;
    Grade grade;
} StudentRecord;

float calculateAverage(int total)
{
    return (float)total / SUBJECT_COUNT;
}

int calculateTotal(int marks1, int marks2, int marks3)
{
    return marks1 + marks2 + marks3;
}

Grade assignGrade(float average)
{
    if (average >= 85) return A;
    else if (average >= 70) return B;
    else if (average >= 50) return C;
    else if (average >= 35) return D;
    else return F;
}

void printStars(Grade grade)
{
    int starCount = 0;
    if (grade == A) starCount = 5;
    else if (grade == B) starCount = 4;
    else if (grade == C) starCount = 3;
    else if (grade == D) starCount = 2;

    for (int i = 0; i < starCount; i++)
    {
        printf("*");
    }
}

void printRollNumbers(int current, int total)
{
    if (current > total) return;

    printf("%d", current);
    if (current < total) printf(" ");

    printRollNumbers(current + 1, total);
}

void processStudent(StudentRecord* student)
{
    student->total = calculateTotal(student->marks[0], student->marks[1], student->marks[2]);
    student->average = calculateAverage(student->total);
    student->grade = assignGrade(student->average);

    printf("Roll: %d\n", student->rollNumber);
    printf("Name: %s\n", student->name);
    printf("Total: %d\n", student->total);
    printf("Average: %.2f\n", student->average);
    printf("Grade: %c\n", student->grade);

    if (student->average < 35)
    {
        printf("\n");
        return;
    }

    printf("Performance: ");
    printStars(student->grade);
    printf("\n\n");
}

int main()
{
    StudentRecord students[MAX_STUDENTS];
    int numberOfStudents = 0;

    scanf("%d", &numberOfStudents);

    for (int i = 0; i < numberOfStudents; i++)
    {
        scanf("%d %s %d %d %d",
              &students[i].rollNumber,
              students[i].name,
              &students[i].marks[0],
              &students[i].marks[1],
              &students[i].marks[2]);

        processStudent(&students[i]);
    }

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(1, numberOfStudents);
    printf("\n");

    return 0;
}