#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "wrap.h"
#include "utils.h"
#include "mq.h"
#include "http.h"
#include "business.h"

#define DEBUG 1
#define SERV_PORT 8010
#define MAX_CLIENT_SIZE 100
#define MAX_USER_SIZE 1000
#define MAX_BUF_SIZE 100
#define CUSTOM_THREAD_NUM 3


#define MAX_HTTP_REQUEST_LEN 2048
#define MAX_HTTP_RESPONSE_LEN 2048

int client_num = 0;
struct mq *mq;
pthread_t tid[CUSTOM_THREAD_NUM];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t p_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_cond = PTHREAD_COND_INITIALIZER;

void init();
void init_socket();

int product(int fd);
void* custom(void*);

int handle_request(struct Message *msg);
void make_response(struct Message *msg);
void print_request(struct Message *msg);
void destory();


int main(int argc, char* argv[]) {
    init();
    init_socket();
    return 0;
}

void init() {
    mq = mq_create(100);
    // product in main thread
    // custom in child thread
    for (int i=0; i<CUSTOM_THREAD_NUM; i++) {
        int ret = pthread_create(&tid[i], NULL, custom, NULL);
        if (ret != 0) {
            printf("Error: pthread_create\n");
            exit(1);
            return;
        }
    }
}

void destory() {
    mq_destory(mq);
}

void init_socket() {
    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in servaddr;
    bzero((void*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, 5);

    int epfd = Epoll_create(1);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    Epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    struct  epoll_event evlist[MAX_CLIENT_SIZE];
    
    while (1) {
        int count = Epoll_wait(epfd, evlist, MAX_CLIENT_SIZE, -1);
        for (int i=0; i<count; i++) {
            struct epoll_event *one = evlist + i;
            if (one->events & EPOLLIN) {
                int fd = one->data.fd;
                if (listenfd == fd) {
                    struct sockaddr_in clientaddr;
                    socklen_t len = sizeof(clientaddr);
                    int clientfd = Accept(fd, (struct sockaddr*)&clientaddr, &len);
                    printf("enter clientfd %d\n", clientfd);

                    struct epoll_event ev;
                    ev.events = EPOLLIN;
                    ev.data.fd = clientfd;
                    Epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
                    client_num++;

                } else {
                    if (product(fd) == 0) {
                        Epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                        Close(fd);
                    }
                }
            }
        }
    }
}

int product(int fd) {
    // TODO:如果数量量大，则多次读取
    char* buf = malloc(MAX_HTTP_REQUEST_LEN);
    bzero(buf, MAX_HTTP_REQUEST_LEN);
    int n = Read(fd, buf, MAX_HTTP_REQUEST_LEN);
    if (n <= 0) return 0;

    pthread_mutex_lock(&mutex);

    while (mq->size >= mq->limit) {
        printf("product thread %lu -->mq is full, to wait\n", pthread_self());
        pthread_cond_wait(&p_cond, &mutex);
    }

    struct Message *msg = malloc(sizeof(struct Message));
    msg->fd = fd;
    msg->msg = buf;
    mq_enqueue(mq, msg);
    // printf("thread %lu enqueue -->%d\n", pthread_self(), msg->fd);
    pthread_cond_broadcast(&c_cond);

    pthread_mutex_unlock(&mutex);

    return n;
}

void* custom(void* x) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (mq->size == 0) {
            printf("custom thread %lu -->mq is empty, to wait\n", pthread_self());
            pthread_cond_wait(&c_cond, &mutex);
        }

        struct Message *msg = mq_dequeue(mq);
        handle_request(msg);
        free(msg);

        pthread_cond_broadcast(&p_cond);

        pthread_mutex_unlock(&mutex);
    }
}

int handle_request(struct Message *msg) {
    if (DEBUG) print_request(msg);

    struct Response resp;
    struct Request req;
    to_request(msg->msg, &req);

    if (req.method == GET) {
        handle_get(&req, &resp);
    } else {
        printf("Error: Unsuport method\n");
    }

    char buf[MAX_HTTP_RESPONSE_LEN];
    bzero(buf, MAX_HTTP_RESPONSE_LEN);
    to_response_text(&resp, buf);
    send_response(msg->fd, buf);

    return 0;
}

void send_response(int fd, char *msg) {
    Write(msg->fd, resp_buf, strlen(msg));
}

void print_request(struct Message *msg) {
    printf("\ncustom %lu %d begin------->\n", pthread_self(), msg->fd);
    printf("%s", msg->msg);
    printf("\ncustom %lu %d end\n", pthread_self(), msg->fd);
}
