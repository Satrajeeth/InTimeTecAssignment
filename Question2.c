#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"
#define MAX_NAME_LENGTH 50

typedef struct
{
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
} UserInfo;

void createFileIfNeeded(void)
{
    FILE* file = fopen(FILE_NAME, "a");
    if (!file)
    {
        printf("Oops! Couldn't create or open the file.\n");
        exit(1);
    }
    fclose(file);
}

void addUser(UserInfo userInfo)
{
    FILE* file = fopen(FILE_NAME, "a");
    if (!file)
    {
        printf("Can't open the file to add a user.\n");
        return;
    }
    fprintf(file, "%d %s %d\n", userInfo.id, userInfo.name, userInfo.age);
    fclose(file);
    printf("User added!\n");
}

void showUsers(void)
{
    FILE* file = fopen(FILE_NAME, "r");
    if (!file)
    {
        printf("No users found (or file can't be opened).\n");
        return;
    }
    
    UserInfo userInfo;
    printf("Current users:\n");
    printf("ID\tName\tAge\n");
    while (fscanf(file, "%d %49s %d", &userInfo.id, userInfo.name, 
                  &userInfo.age) != EOF)
    {
        printf("%d\t%s\t%d\n", userInfo.id, userInfo.name, userInfo.age);
    }
    fclose(file);
}

void updateUser(int userId, UserInfo newUserData)
{
    FILE* file = fopen(FILE_NAME, "r");
    FILE* tempFile = fopen("temp.txt", "w");
    if (!file || !tempFile)
    {
        printf("Can't open file to update.\n");
        return;
    }
    
    UserInfo userInfo;
    int isFound = 0;
    while (fscanf(file, "%d %49s %d", &userInfo.id, userInfo.name, 
                  &userInfo.age) != EOF)
    {
        if (userInfo.id == userId)
        {
            fprintf(tempFile, "%d %s %d\n", newUserData.id, newUserData.name, 
                    newUserData.age);
            isFound = 1;
        }
        else
        {
            fprintf(tempFile, "%d %s %d\n", userInfo.id, userInfo.name, 
                    userInfo.age);
        }
    }
    fclose(file);
    fclose(tempFile);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (isFound)
    {
        printf("User updated!\n");
    }
    else
    {
        printf("Couldn't find user with ID %d :(\n", userId);
    }
}

void deleteUser(int userId)
{
    FILE* file = fopen(FILE_NAME, "r");
    FILE* tempFile = fopen("temp.txt", "w");
    if (!file || !tempFile)
    {
        printf("Can't open file to delete.\n");
        return;
    }
    
    UserInfo userInfo;
    int isFound = 0;
    while (fscanf(file, "%d %49s %d", &userInfo.id, userInfo.name, 
                  &userInfo.age) != EOF)
    {
        if (userInfo.id == userId)
        {
            isFound = 1;
        }
        else
        {
            fprintf(tempFile, "%d %s %d\n", userInfo.id, userInfo.name, 
                    userInfo.age);
        }
    }
    fclose(file);
    fclose(tempFile);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (isFound)
    {
        printf("User deleted!\n");
    }
    else
    {
        printf("User with ID %d not found.\n", userId);
    }
}

int main(void)
{
    createFileIfNeeded();

    int choice = 0;
    UserInfo userInfo;
    int userId = 0;

    do
    {
        printf("\nMenu:\n");
        printf("1. Add a user\n");
        printf("2. Show users\n");
        printf("3. Update a user\n");
        printf("4. Delete a user\n");
        printf("5. Quit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("User ID: ");
                scanf("%d", &userInfo.id);
                printf("Name: ");
                scanf("%49s", userInfo.name);
                printf("Age: ");
                scanf("%d", &userInfo.age);
                addUser(userInfo);
                break;
            case 2:
                showUsers();
                break;
            case 3:
                printf("ID to update: ");
                scanf("%d", &userId);
                printf("New ID: ");
                scanf("%d", &userInfo.id);
                printf("New Name: ");
                scanf("%49s", userInfo.name);
                printf("New Age: ");
                scanf("%d", &userInfo.age);
                updateUser(userId, userInfo);
                break;
            case 4:
                printf("ID to delete: ");
                scanf("%d", &userId);
                deleteUser(userId);
                break;
            case 5:
                printf("Bye!\n");
                break;
            default:
                printf("Hmm, that's not an option.\n");
        }
    } while (choice != 5);
    
    return 0;
}