#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREAD 100

void *thread_inc(void *arg);
void *thread_des(void *arg);

long long num = 0;
pthread_mutex_t mutex;

int main(int argc, char *argv[]) {
    pthread_t thread_id[NUM_THREAD];
    int i;

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("mutex init failed");
        return EXIT_FAILURE;
    }

    /* 스레드 100개 생성: 홀수는 증가, 짝수는 감소 */
    for (i = 0; i < NUM_THREAD; i++) {
        if (i % 2)
            pthread_create(&thread_id[i], NULL, thread_inc, NULL);
        else
            pthread_create(&thread_id[i], NULL, thread_des, NULL);
    }

    /* 모든 스레드 종료 대기 */
    for (i = 0; i < NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);

    printf("result : %lld\n", num);

    pthread_mutex_destroy(&mutex);
    return 0;
}

void *thread_inc(void *arg) {
    int i;

    /* 공유 변수 num에 안전하게 접근 */
    pthread_mutex_lock(&mutex);
    for (i = 0; i < 50000000; i++) {
        num += 1;
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void *thread_des(void *arg) {
    int i;

    /* 공유 변수 num에 안전하게 접근 */
    pthread_mutex_lock(&mutex);
    for (i = 0; i < 50000000; i++) {
        num -= 1;
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}
