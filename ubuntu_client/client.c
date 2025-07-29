// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_IP   "192.168.0.10"  // Raspberry Pi IP
#define SERVER_PORT 5000
#define BUF_SIZE    512

static int sock_fd;

// 서버로부터 들어오는 메시지 수신 스레드
void *recv_thread(void *arg) {
    char buf[BUF_SIZE];
    int n;
    while ((n = read(sock_fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
    }
    return NULL;
}

int main() {
    struct sockaddr_in serv_addr;
    pthread_t tid;

    // 소켓 생성
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock_fd);
        return 1;
    }

    // 서버 연결
    if (connect(sock_fd, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sock_fd);
        return 1;
    }
    printf("Connected to %s:%d\n", SERVER_IP, SERVER_PORT);

    // 수신 스레드 시작
    pthread_create(&tid, NULL, recv_thread, NULL);

    // 표준 입력으로부터 명령 입력 및 전송
    char line[BUF_SIZE];
    while (fgets(line, sizeof(line), stdin)) {
        write(sock_fd, line, strlen(line));
    }

    close(sock_fd);
    return 0;
}
