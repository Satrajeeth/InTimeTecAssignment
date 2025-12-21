#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

struct my_msgbuf {
    long mtype;
    int n;
    int arr[100];
};

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
    key_t key = ftok("msg", 65);
    int mqid = msgget(key, 0666 | IPC_CREAT);

    struct my_msgbuf m;

    printf("Enter number of elements: ");
    scanf("%d", &m.n);

    printf("Enter elements:\n");
    for(int i = 0; i < m.n; i++)
        scanf("%d", &m.arr[i]);

    printf("Before Sorting: ");
    for(int i = 0; i < m.n; i++)
        printf("%d ", m.arr[i]);
    printf("\n");

    m.mtype = 1;
    msgsnd(mqid, &m, sizeof(m) - sizeof(long), 0);

    if(fork() == 0) {
        msgrcv(mqid, &m, sizeof(m) - sizeof(long), 1, 0);
        sort(m.arr, m.n);
        m.mtype = 2;
        msgsnd(mqid, &m, sizeof(m) - sizeof(long), 0);
        return 0;
    }

    msgrcv(mqid, &m, sizeof(m) - sizeof(long), 2, 0);

    printf("After Sorting: ");
    for(int i = 0; i < m.n; i++)
        printf("%d ", m.arr[i]);
    printf("\n");

    msgctl(mqid, IPC_RMID, NULL);
    return 0;
}
