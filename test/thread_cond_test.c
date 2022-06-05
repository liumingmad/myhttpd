#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "../mq.h"

void* product(void* x);
void* custom(void* x);

static int a = 0;
int b = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t p_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_cond = PTHREAD_COND_INITIALIZER;
struct mq *mq = NULL;

int main(int argc, char* argv[]) {
    mq = mq_create(10);

    int x = 0;
    int size = 3;
    pthread_t tid[size];
    for (int i=0; i<size; i++) {
        int ret = pthread_create(&tid[i], NULL, product, &x);
        if (ret != 0) {
            printf("pthread_create error");
            return 1;
        }
    }

    int x1 = 0;
    int size1 = 3;
    pthread_t tid1[size];
    for (int i=0; i<size1; i++) {
        int ret = pthread_create(&tid1[i], NULL, custom, &x1);
        if (ret != 0) {
            printf("pthread_create error");
            return 1;
        }
    }

    for (int i=0; i<size; i++) {
        pthread_join(tid1[i], NULL);
    }

    for (int i=0; i<size; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("%d  %d\n", a, b);
    return 0;
}

void* product(void* x) {
    while (1) {
        sleep(1);
        pthread_mutex_lock(&mutex);
        while (mq->size >= mq->limit) {
            printf("thread %lu -->mq is full, to wait\n", pthread_self());
            pthread_cond_wait(&p_cond, &mutex);
        }
        struct Message *m = malloc(sizeof(struct Message)); 
        m->fd = a++;
        m->msg = "kkkkk";
        mq_enqueue(mq, m);
        printf("thread %lu enqueue -->%d\n", pthread_self(), m->fd);
        pthread_cond_broadcast(&c_cond);
        pthread_mutex_unlock(&mutex);
    }
}

void* custom(void* x) {
    while (1) {
        sleep(1);
        pthread_mutex_lock(&mutex);
        while (mq->size == 0) {
            printf("thread %lu -->mq is empty, to wait\n", pthread_self());
            pthread_cond_wait(&c_cond, &mutex);
        }
        struct Message *m = mq_dequeue(mq);
        printf("thread %lu dequeue -->%d  %s\n", pthread_self(), m->fd, m->msg);
        free(m);
        pthread_cond_broadcast(&p_cond);
        pthread_mutex_unlock(&mutex);
    }
}