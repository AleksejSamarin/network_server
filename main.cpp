#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>  //
#include <netinet/in.h> //
#include <arpa/inet.h>  //
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <poll.h>

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

    struct pollfd poll_struct[SOMAXCONN];
    poll_struct[0].fd = sockDesc;
    poll_struct[0].events = POLLIN;
    int index = 1;

    while (true) {
        if (poll(poll_struct, SOMAXCONN, 0) > 0) {
            if ((poll_struct[0].revents & POLLIN) > 0) {
                int conn_fd = accept(sockDesc, NULL, NULL);
                printf("New connection\n");
                poll_struct[index].fd = conn_fd;
                poll_struct[index].events = POLLIN;
                index++;
            }
            for (int i = 1; i < index; i++) {
                if ((poll_struct[i].revents & POLLIN) > 0) {
                    ssize_t k = read(poll_struct[i].fd, buffer, size);
                    if (k <= 0) {
                        printf("Client disconnected\n");
                        close(poll_struct[i].fd);
                        //todo: delete, container
                        break;
                    }
                    printf("Message received: %s", buffer);
                    write(poll_struct[i].fd, hello, strlen(hello));
                }
            }
        }
    }
}