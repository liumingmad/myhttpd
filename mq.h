
struct Message {
    int fd;
    char* msg;
};

struct mq {
    struct Message **data;
    int limit;
    int head;
    int tail;
    int size;
};


struct mq* mq_create();

void mq_destory(struct mq *mq);

int mq_size();

int mq_enqueue(struct mq *mq, struct Message* m);

struct Message* mq_dequeue(struct mq *mq);

void mq_print(struct mq* mq);