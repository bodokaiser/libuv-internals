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
    QUEUE node;
};

void * worker();

void submit_work(int a, int b, int type);

int main() {
    QUEUE_INIT(&queue);

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    struct work_s work[2];

    work[0].a = 5;
    work[0].b = 7;
    work[0].type = 1;

    work[1].a = 3;
    work[1].b = 3;
    work[1].type = 3;

    QUEUE_INIT(&work[0].node);
    QUEUE_INIT(&work[1].node);

    QUEUE_INSERT_TAIL(&queue, &work[0].node);
    QUEUE_INSERT_TAIL(&queue, &work[1].node);

    /* this does actually the same as above but causes a segmentation fault. */
    submit_work(5, 6, 3);
        
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

void submit_work(int a, int b, int type) {
    struct work_s work;

    work.a = a;
    work.b = b;
    work.type = type;

    pthread_mutex_lock(&mutex);

    QUEUE_INIT(&work.node);
    QUEUE_INSERT_TAIL(&queue, &work.node);

    pthread_mutex_unlock(&mutex);

    pthread_cond_signal(&cond);
}

void * worker() { 
    QUEUE * q;

    int result;

    struct work_s * work;

    for (;;) {
        while (QUEUE_EMPTY(&queue)) {
            printf("waiting for insertion\n");
            pthread_cond_wait(&cond, &mutex);
        }
        
        pthread_mutex_lock(&mutex);

        q = QUEUE_HEAD(&queue);
        
        /* here the segmentation fault occurs */
        QUEUE_REMOVE(q);

        pthread_mutex_unlock(&mutex);
        
        work = QUEUE_DATA(q, struct work_s, node);
        
        /* this will print some incorrect data for work */
        printf("received work type %d with a %d and b %d \n", work->a, work->b, work->type);
      
        if (work->type == 0) {
            break;
        }

        switch (work->type) {
            case 1:
                result = work->a + work->b;
                printf("%d + %d = %d\n", work->a, work->b, result);
                break;
            case 2:
                result = work->a - work->b;
                printf("%d - %d = %d\n", work->a, work->b, result);
                break;
            case 3:
                result = work->a * work->b;
                printf("%d * %d = %d\n", work->a, work->b, result);
                break;
            case 4:
                result = work->a / work->b;
                printf("%d / %d = %d\n", work->a, work->b, result);
                break;
        }
    }


    pthread_exit(NULL);
}
