#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

void* fun(void* x);

static int a = 0;
int b = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]) {
    int x = 0;
    int size = 3;
    pthread_t tid[size];
    for (int i=0; i<size; i++) {
        int ret = pthread_create(&tid[i], NULL, fun, &x);
        if (ret != 0) {
            printf("pthread_create error");
            return 1;
        }
    }

    for (int i=0; i<size; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("%d  %d\n", a, b);
    return 0;
}

void* fun(void* x) {
    int y = 0;
    for (int i=0; i<100000; i++) {
        //a++;
        //__sync_fetch_and_add(&a, 1);
        pthread_mutex_lock(&mutex);
        a++;
        pthread_mutex_unlock(&mutex);
        //printf("thread %lu -->%d\t%d\n", pthread_self(), a, b);
    }
}