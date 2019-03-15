#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <iconv.h>

int main() {
    struct sockaddr_in server = {};
    socklen_t len = sizeof(struct sockaddr_in);
    int sockDesc;
    const int size = 1024;
    char buffer1[size], buffer2[size];

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
                    ssize_t k = read(poll_struct[i].fd, buffer1, size);
                    if (k <= 0) {
                        printf("Client disconnected\n");
                        close(poll_struct[i].fd);
                        for (int j = i; j < index; j++) {
                            poll_struct[j] = poll_struct[j + 1];
                        }
                        index--;
                        break;
                    }
                    size_t src_len = size;
                    size_t dst_len = size;
                    char *p_in = buffer1;
                    char *p_out = buffer2;
                    iconv_t conv = iconv_open("ASCII","CP1251");
                    iconv(conv, &p_in, &src_len, &p_out, &dst_len);
                    iconv_close(conv);
                    printf("Message received: %s", buffer1);
                    write(poll_struct[i].fd, buffer2, strlen(buffer2));
                }
            }
        }
    }
}