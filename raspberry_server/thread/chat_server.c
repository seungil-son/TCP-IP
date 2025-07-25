#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // memset(), strcmp() 등
#include <unistd.h>         // close(), write(), read(), sleep()
#include <arpa/inet.h>      // inet_ntoa(), htons(), htonl()
#include <sys/socket.h>     // socket(), bind(), listen(), accept()
#include <netinet/in.h>     // struct sockaddr_in
#include <pthread.h>        // pthread_xxx()

#define BUF_SIZE   100
#define MAX_CLNT   256

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(const char *msg);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 뮤텍스 초기화
    pthread_mutex_init(&mutx, NULL);

    // 서버 소켓 생성
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0) 
        error_handling("socket() error");

    // 주소 정보 세팅
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family      = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port        = htons(atoi(argv[1]));

    // bind
    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) < 0)
        error_handling("bind() error");

    // listen
    if (listen(serv_sock, 5) < 0)
        error_handling("listen() error");

    puts("=== 서버 시작 ===");

    while (1) {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock,
                           (struct sockaddr*)&clnt_adr,
                           &clnt_adr_sz);
        if (clnt_sock < 0) 
            error_handling("accept() error");

        // 신규 클라이언트 추가
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        // 클라이언트 소켓을 동적 할당해서 스레드에 넘김
        int *pclient = malloc(sizeof(int));
        *pclient = clnt_sock;
        pthread_create(&t_id, NULL, handle_clnt, pclient);
        pthread_detach(t_id);  // 스레드 자동 자원 해제

        printf("접속: %s\n", inet_ntoa(clnt_adr.sin_addr));
    }

    close(serv_sock);
    pthread_mutex_destroy(&mutx);
    return 0;
}

void *handle_clnt(void *arg) {
    int sock = *((int*)arg);
    free(arg);

    char msg[BUF_SIZE];
    int str_len;

    // 클라이언트로부터 메시지 읽어서 브로드캐스트
    while ((str_len = read(sock, msg, sizeof(msg))) > 0) {
        send_msg(msg, str_len);
    }

    // 클라이언트 연결 종료 처리
    pthread_mutex_lock(&mutx);
    // clnt_socks 배열에서 sock 삭제
    for (int i = 0; i < clnt_cnt; i++) {
        if (clnt_socks[i] == sock) {
            while (i < clnt_cnt - 1) {
                clnt_socks[i] = clnt_socks[i+1];
                i++;
            }
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(sock);

    return NULL;
}

void send_msg(char *msg, int len) {
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++) {
        write(clnt_socks[i], msg, len);
    }
    pthread_mutex_unlock(&mutx);
}

void error_handling(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
