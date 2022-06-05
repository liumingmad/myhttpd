#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "mq.h"


void msg_print(struct Message* msg);
void mq_print(struct mq* mq);

void* count(void* x);
void* fun(void *mq);
void* productor(void *mq);
void* customer(void *mq);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// __sync_fetch_and_add(&count, 1);
// __sync_fetch_and_sub(&count, 1);
int main(int argc, char *argv[]) {
    struct mq* mq = mq_create(1000);
    for (int i=0; i<1000; i++) {
        struct Message *m = malloc(sizeof(struct Message));
        m->fd = i;
        m->msg = "0000000";
        mq_enqueue(mq, m);
    }

    printf("main create thread\n");
    int len = 10;
    pthread_t tid[len]; 
    for (int i=0; i<len; i++) {
        int ret = pthread_create(&tid[i], NULL, fun, (void*)mq);
        printf("tid=%lu, ret=%d\n", (unsigned long)tid[i], ret);
    }

    for (int i=0; i<len; i++) {
        void* val;
        pthread_join(tid[i], &val);
        printf("main finish %d\n", *(int*)val);
    }

    printf("mq size %d\n", mq->size);

    return 0;
}

void* fun(void *p) {
    printf("thread %lu ---> start\n", pthread_self());
    struct mq *mq = (struct mq *)p; 
    static int fd;

    //pthread_mutex_lock(&mutex);
    //printf("thread %lu ---> get lock\n", pthread_self());
    for (int i=0; i<100; i++) {
        struct Message* msg = mq_dequeue(mq);
        printf("thread %lu --->", pthread_self());
        msg_print(msg);
        fd = msg->fd;
    }
    //pthread_mutex_unlock(&mutex);

    printf("thread %lu ---> stop\n", pthread_self());
    pthread_exit((void*)&fd);
}

void test2() {

}

void test1(struct mq *mq) {
    struct Message *m0 = malloc(sizeof(struct Message));
    m0->fd = 0;
    m0->msg = "kdkkdkkd";
    mq_enqueue(mq, m0);

    struct Message *m1 = malloc(sizeof(struct Message));
    m1->fd = 1;
    m1->msg = "asasasa";
    mq_enqueue(mq, m1);

    struct Message *m2 = malloc(sizeof(struct Message));
    m2->fd = 2;
    m2->msg = "kklsllllllll";
    mq_enqueue(mq, m2);

    mq_print(mq);

    printf("---------\n");
    msg_print(mq_dequeue(mq));
    msg_print(mq_dequeue(mq));
    mq_print(mq);

    struct Message *m3 = malloc(sizeof(struct Message));
    m3->fd = 3;
    m3->msg = "kklsllllllll";
    mq_enqueue(mq, m3);

    struct Message *m4 = malloc(sizeof(struct Message));
    m4->fd = 4;
    m4->msg = "kklsllllllll";
    mq_enqueue(mq, m4);


    mq_print(mq);

    printf("---------\n");
    msg_print(mq_dequeue(mq));
    msg_print(mq_dequeue(mq));

    struct Message *m5 = malloc(sizeof(struct Message));
    m5->fd = 5;
    m5->msg = "kklsllllllll";
    mq_enqueue(mq, m5);
    mq_print(mq);
}

void msg_print(struct Message* msg) {
    printf("%d %s\n", msg->fd, msg->msg);
}

void mq_print(struct mq* mq) {
    printf("size=%d\t", mq->size);
    printf("head=%d\t", mq->head);
    printf("tail=%d\t", mq->tail);
    printf("limit=%d\t\n", mq->limit);

    int i = 0;
    while (i < mq->size) {
        int index = (mq->head + i) % mq->limit;
        struct Message* msg = mq->data[index];
        printf("%d %s\n", msg->fd, msg->msg);
        i++;
    }
}