#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mq.h"

struct mq* mq_create(int limit) {
    if (limit <= 0) {
        perror("Error: limit must beyond 0!");
        return NULL;
    } 

    struct mq* mq = malloc(sizeof(struct mq));
    mq->head = 0;
    mq->tail = 0;
    mq->size = 0;
    mq->limit = limit;

    int len = limit * sizeof(struct Message*);
    mq->data = malloc(len);
    bzero(mq->data, len);
    return mq;
}

// 互斥锁
void mq_destory(struct mq *mq) {
    if (mq == NULL) {
        printf("Error: mq is NULL\n");
        return;
    }

    for (int i=0; i<mq->limit; i++) {
        if (mq->data[i]) free(mq->data[i]);
    }
    free(mq->data);
    mq->data = NULL;
    mq->head = 0;
    mq->tail = 0;
    mq->size = 0;
    free(mq);
}

int mq_size(struct mq *mq) {
    return mq->size;
}

// 条件变量
int mq_enqueue(struct mq *mq, struct Message* m) {
    if (mq == NULL) {
        perror("Error: mq is NULL");
        return 1;
    } 

    if (m == NULL) {
        perror("Error: message is NULL");
        return 2;
    } 

    if (mq->size >= mq->limit) {
        perror("Error: mq is full");
        return 3; 
    }

    mq->data[mq->tail] = m;
    mq->size++;
    mq->tail = (mq->tail + 1) % mq->limit;
    return 0;
}

// 条件变量
struct Message* mq_dequeue(struct mq *mq) {
    if (mq == NULL) {
        perror("Error: mq is NULL");
        return NULL;
    } 

    if (mq->size == 0) {
        perror("Error: mq is empty");
        return NULL;
    } 

    struct Message *one = mq->data[mq->head];
    mq->head = (mq->head + 1) % mq->limit;
    mq->size--;
    return one;
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