#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <netinet/in.h>

pthread_mutex_t lock;

void* handle_client(void* arg) {
    int sock = *(int*)arg;
    int choice, amount, balance;

    pthread_mutex_lock(&lock);
    FILE *fp = fopen("accountDB.txt", "r");
    fscanf(fp, "%d", &balance);
    fclose(fp);
    pthread_mutex_unlock(&lock);

    recv(sock, &choice, sizeof(choice), 0);

    if(choice == 1 || choice == 2)
        recv(sock, &amount, sizeof(amount), 0);

    pthread_mutex_lock(&lock);

    if(choice == 1) {
        if(amount > balance)
            balance = -1;
        else
            balance -= amount;
    } else if(choice == 2) {
        balance += amount;
    }

    fp = fopen("accountDB.txt", "w");
    fprintf(fp, "%d", balance);
    fclose(fp);

    pthread_mutex_unlock(&lock);

    send(sock, &balance, sizeof(balance), 0);
    close(sock);
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    pthread_t tid;

    pthread_mutex_init(&lock, NULL);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("ATM Server Running...\n");

    while(1) {
        client_fd = accept(server_fd, NULL, NULL);
        pthread_create(&tid, NULL, handle_client, &client_fd);
        pthread_detach(tid);
    }
}
