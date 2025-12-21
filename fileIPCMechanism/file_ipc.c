#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void sort(int a[], int n) {
    for(int i = 0; i < n-1; i++)
        for(int j = 0; j < n-i-1; j++)
            if(a[j] > a[j+1]) {
                int t = a[j];
                a[j] = a[j+1];
                a[j+1] = t;
            }
}

int main() {
    int n;
    printf("Enter number of elements: ");
    scanf("%d", &n);

    int arr[n];
    printf("Enter elements:\n");
    for(int i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    printf("Before Sorting: ");
    for(int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    FILE *fp = fopen("data.txt", "w");
    fprintf(fp, "%d\n", n);
    for(int i = 0; i < n; i++)
        fprintf(fp, "%d ", arr[i]);
    fclose(fp);

    if(fork() == 0) {
        fp = fopen("data.txt", "r");
        fscanf(fp, "%d", &n);
        for(int i = 0; i < n; i++)
            fscanf(fp, "%d", &arr[i]);
        fclose(fp);

        sort(arr, n);

        fp = fopen("data.txt", "w");
        fprintf(fp, "%d\n", n);
        for(int i = 0; i < n; i++)
            fprintf(fp, "%d ", arr[i]);
        fclose(fp);
        exit(0);
    }

    wait(NULL);

    fp = fopen("data.txt", "r");
    fscanf(fp, "%d", &n);
    for(int i = 0; i < n; i++)
        fscanf(fp, "%d", &arr[i]);
    fclose(fp);

    printf("After Sorting: ");
    for(int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    return 0;
}
