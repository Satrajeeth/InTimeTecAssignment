#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"

typedef struct {
    int id;
    char name[50];
    int age;
} UserInfo;

void create_file_if_needed() {
    FILE *file = fopen(FILE_NAME, "a");
    if (!file) {
        printf("Oops! Couldn't create or open the file.\n");
        exit(1);
    }
    fclose(file);
}

void add_user(UserInfo user_info) {
    FILE *file = fopen(FILE_NAME, "a");
    if (!file) {
        printf("Can't open the file to add a user.\n");
        return;
    }
    fprintf(file, "%d %s %d\n", user_info.id, user_info.name, user_info.age);
    fclose(file);
    printf("User added!\n");
}

void show_users() {
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) {
        printf("No users found (or file can't be opened).\n");
        return;
    }
    UserInfo user_info;
    printf("Current users:\n");
    printf("ID\tName\tAge\n");
    while (fscanf(file, "%d %49s %d", &user_info.id, user_info.name, &user_info.age) != EOF) {
        printf("%d\t%s\t%d\n", user_info.id, user_info.name, user_info.age);
    }
    fclose(file);
}

void update_user(int user_id, UserInfo new_user_data) {
    FILE *file = fopen(FILE_NAME, "r");
    FILE *temp_file = fopen("temp.txt", "w");
    if (!file || !temp_file) {
        printf("Can't open file to update.\n");
        return;
    }
    UserInfo user_info;
    int is_found = 0;
    while (fscanf(file, "%d %49s %d", &user_info.id, user_info.name, &user_info.age) != EOF) {
        if (user_info.id == user_id) {
            fprintf(temp_file, "%d %s %d\n", new_user_data.id, new_user_data.name, new_user_data.age);
            is_found = 1;
        } else {
            fprintf(temp_file, "%d %s %d\n", user_info.id, user_info.name, user_info.age);
        }
    }
    fclose(file);
    fclose(temp_file);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (is_found)
        printf("User updated!\n");
    else
        printf("Couldn't find user with ID %d :(\n", user_id);
}

void delete_user(int user_id) {
    FILE *file = fopen(FILE_NAME, "r");
    FILE *temp_file = fopen("temp.txt", "w");
    if (!file || !temp_file) {
        printf("Can't open file to delete.\n");
        return;
    }
    UserInfo user_info;
    int is_found = 0;
    while (fscanf(file, "%d %49s %d", &user_info.id, user_info.name, &user_info.age) != EOF) {
        if (user_info.id == user_id) {
            is_found = 1;
        } else {
            fprintf(temp_file, "%d %s %d\n", user_info.id, user_info.name, user_info.age);
        }
    }
    fclose(file);
    fclose(temp_file);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (is_found)
        printf("User deleted!\n");
    else
        printf("User with ID %d not found.\n", user_id);
}

int main() {
    create_file_if_needed();

    int choice;
    UserInfo user_info;
    int user_id;

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
                scanf("%d", &user_info.id);
                printf("Name: ");
                scanf("%49s", user_info.name);
                printf("Age: ");
                scanf("%d", &user_info.age);
                add_user(user_info);
                break;
            case 2:
                show_users();
                break;
            case 3:
                printf("ID to update: ");
                scanf("%d", &user_id);
                printf("New ID: ");
                scanf("%d", &user_info.id);
                printf("New Name: ");
                scanf("%49s", user_info.name);
                printf("New Age: ");
                scanf("%d", &user_info.age);
                update_user(user_id, user_info);
                break;
            case 4:
                printf("ID to delete: ");
                scanf("%d", &user_id);
                delete_user(user_id);
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
