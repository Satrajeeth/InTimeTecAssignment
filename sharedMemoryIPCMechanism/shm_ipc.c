#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
    key_t key = ftok("shm", 65);
    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    int *data = (int*) shmat(shmid, NULL, 0);

    int n;
    printf("Enter number of elements: ");
    scanf("%d", &n);
    data[0] = n;

    printf("Enter elements:\n");
    for(int i = 1; i <= n; i++)
        scanf("%d", &data[i]);

    printf("Before Sorting: ");
    for(int i = 1; i <= n; i++)
        printf("%d ", data[i]);
    printf("\n");

    if(fork() == 0) {
        sort(&data[1], data[0]);
        return 0;
    }

    wait(NULL);

    printf("After Sorting: ");
    for(int i = 1; i <= n; i++)
        printf("%d ", data[i]);
    printf("\n");

    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
