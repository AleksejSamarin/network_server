#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>  //
#include <netinet/in.h> //
#include <arpa/inet.h>  //
#include <unistd.h>
#include <cstdio>
#include <cstring>

int main() {
    struct sockaddr_in server = {};
    socklen_t len = sizeof(struct sockaddr_in);
    int sockDesc;
    const int size = 1024;
    char buffer[size];
    char hello[] = "From server";

    server.sin_family = AF_INET;
    server.sin_port = htons(2345);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if ((sockDesc = socket(AF_INET, SOCK_STREAM, NULL)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }
    if (bind(sockDesc, (struct sockaddr*)&server, len) < 0) {
        printf("Failed to bind socket\n");
    }
    listen(sockDesc, SOMAXCONN);

    while (true) {
        int connfd = accept(sockDesc, NULL, NULL);
        printf("New connection\n");
        while (true) {
            ssize_t k = read(connfd, buffer, size);
            if (k <= 0) {
                printf("Client disconnected\n");
                close(connfd);
                break;
            }
            printf("Message received: %s", buffer);
            write(connfd, hello, strlen(hello));
        }
    }
}