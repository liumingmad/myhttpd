#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

int a = 0;
sem_t sem;

void* fun(void* arg);

int main(int argc, char *argv[]) {
    sem_init(&sem, 0, 1);

    int x = 0;
    int size = 3;
    pthread_t tid[size];
    for (int i=0; i<size; i++) {
        int ret = pthread_create(&tid[i], NULL, fun, &x);
        if (ret != 0) {
            printf("pthread_create error\n");
            return 1;
        }
    }

    for (int i=0; i<size; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("%d\n", a);

    sem_destroy(&sem);
    return 0;
}

void* fun(void* arg) {
    for (int i=0; i<100000; i++) {
        sem_wait(&sem);
        a++;
        sem_post(&sem);
    }
}