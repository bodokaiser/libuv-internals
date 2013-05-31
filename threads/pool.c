#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../queue/queue.h"

#define MAX_THREADS 1

pthread_t threads[MAX_THREADS];

pthread_cond_t cond;

pthread_mutex_t mutex;

QUEUE queue;

struct work_s {
    int a;
    int b;
    int type;
    QUEUE node[2];
};

void * worker();

int submit_work(int a, int b, int type);

int main() {
    QUEUE_INIT(&queue);

    submit_work(5, 5, 1);
    submit_work(0, 0, 0);

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&threads[i], NULL, worker, NULL);

    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(threads[i], NULL);

    for (int i = 0; i < MAX_THREADS; i++)
        pthread_detach(threads[i]);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}

void * worker() { 
    QUEUE * q;

    struct work_s * work;

    int result;

    for (;;) {
        pthread_mutex_lock(&mutex);

        while (QUEUE_EMPTY(&queue))
            pthread_cond_wait(&cond, &mutex);

        q = QUEUE_HEAD(&queue);

        QUEUE_REMOVE(q);

        pthread_mutex_unlock(&mutex);

        work = QUEUE_DATA(q, struct work_s, node);

        if (work->type == 0) {
            pthread_exit(NULL);
            break;
        }

        switch (work->type) {
            case 1:
                result = work->a + work->b;
                printf("%d + %d = %d", work->a, work->b, result);
                break;
            case 2:
                result = work->a - work->b;
                printf("%d - %d = %d", work->a, work->b, result);
                break;
            case 3:
                result = work->a * work->b;
                printf("%d * %d = %d", work->a, work->b, result);
                break;
            case 4:
                result = work->a / work->b;
                printf("%d / %d = %d", work->a, work->b, result);
                break;
        }
    }
}

int submit_work(int a, int b, int type) {
    struct work_s work;

    work.a = a;
    work.b = b;
    work.type = type;

    pthread_mutex_lock(&mutex);

    QUEUE_INIT(work.node);
    QUEUE_INSERT_TAIL(work.node, &queue);

    pthread_mutex_unlock(&mutex);

    pthread_cond_signal(&cond);

    return 0;
}
