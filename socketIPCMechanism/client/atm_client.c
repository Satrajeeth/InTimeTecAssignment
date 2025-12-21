#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>

int main() {
    int sock, choice, amount, result;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    printf("1. Withdraw\n2. Deposit\n3. Display Balance\n4. Exit\n");
    scanf("%d", &choice);

    send(sock, &choice, sizeof(choice), 0);

    if(choice == 1 || choice == 2) {
        printf("Enter amount: ");
        scanf("%d", &amount);
        send(sock, &amount, sizeof(amount), 0);
    }

    recv(sock, &result, sizeof(result), 0);

    if(result == -1)
        printf("Insufficient Balance\n");
    else
        printf("Current Balance: %d\n", result);

    close(sock);
    return 0;
}
