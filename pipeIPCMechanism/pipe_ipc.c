#include <stdio.h>
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
    int fd1[2], fd2[2];
    pipe(fd1);
    pipe(fd2);

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

    if(fork() == 0) {
        read(fd1[0], &n, sizeof(n));
        read(fd1[0], arr, sizeof(arr));

        sort(arr, n);

        write(fd2[1], arr, sizeof(arr));
        return 0;
    } else {
        write(fd1[1], &n, sizeof(n));
        write(fd1[1], arr, sizeof(arr));

        wait(NULL);
        read(fd2[0], arr, sizeof(arr));

        printf("After Sorting: ");
        for(int i = 0; i < n; i++)
            printf("%d ", arr[i]);
        printf("\n");
    }
    return 0;
}
