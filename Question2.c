#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "users.txt"

typedef struct {
    int id;
    char name[50];
    int age;
} User;

void createFileIfNeeded() {
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        printf("Oops! Couldn't create or open the file.\n");
        exit(1);
    }
    fclose(file);
}

void addUser(User user) {
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        printf("Can't open the file to add a user.\n");
        return;
    }
    fprintf(file, "%d %s %d\n", user.id, user.name, user.age);
    fclose(file);
    printf("User added!\n");
}

void showUsers() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("No users found (or file can't be opened).\n");
        return;
    }
    User user;
    printf("Current users:\n");
    printf("ID\tName\tAge\n");
    while (fscanf(file, "%d %s %d", &user.id, user.name, &user.age) != EOF) {
        printf("%d\t%s\t%d\n", user.id, user.name, user.age);
    }
    fclose(file);
}

void updateUser(int id, User newUserData) {
    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("Can't open file to update.\n");
        return;
    }
    User user;
    int found = 0;
    while (fscanf(file, "%d %s %d", &user.id, user.name, &user.age) != EOF) {
        if (user.id == id) {
            fprintf(temp, "%d %s %d\n", newUserData.id, newUserData.name, newUserData.age);
            found = 1;
        } else {
            fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
        }
    }
    fclose(file);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("User updated!\n");
    else
        printf("Couldn't find user with ID %d :(\n", id);
}

void deleteUser(int id) {
    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("Can't open file to delete.\n");
        return;
    }
    User user;
    int found = 0;
    while (fscanf(file, "%d %s %d", &user.id, user.name, &user.age) != EOF) {
        if (user.id == id) {
            found = 1;  
        } else {
            fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
        }
    }
    fclose(file);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("User deleted!\n");
    else
        printf("User with ID %d not found.\n", id);
}

int main() {
    createFileIfNeeded();

    int choice;
    User user;
    int id;

    do {
        printf("\nMenu:\n");
        printf("1. Add a user\n");
        printf("2. Show users\n");
        printf("3. Update a user\n");
        printf("4. Delete a user\n");
        printf("5. Quit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                printf("User ID: ");
                scanf("%d", &user.id);
                printf("Name: ");
                scanf("%s", user.name);
                printf("Age: ");
                scanf("%d", &user.age);
                addUser(user);
                break;
            case 2:
                showUsers();
                break;
            case 3:
                printf("ID to update: ");
                scanf("%d", &id);
                printf("New ID: ");
                scanf("%d", &user.id);
                printf("New Name: ");
                scanf("%s", user.name);
                printf("New Age: ");
                scanf("%d", &user.age);
                updateUser(id, user);
                break;
            case 4:
                printf("ID to delete: ");
                scanf("%d", &id);
                deleteUser(id);
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
