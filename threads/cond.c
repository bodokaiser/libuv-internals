#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/**
 * Our threads.
 */
pthread_t threads[3];

/**
 * Condition object.
 *
 * The condition object is used to inform a thread about the current
 * state of a specific ressource. It can be used to publish more
 * specific information about operations which can/should be
 * performed by a thread. 
 *
 * For example there are two threads: Thread one creates some random
 * data for crypto. Thread two can decode this binary data to base64.
 * Now thread two needs to now when thread one is finished. To achieve
 * this we use thread conditions.
 */
pthread_cond_t cond;

/**
 * Mutex lock object.
 */
pthread_mutex_t mutex;

/**
 * External log flag.
 */
int worker_two_flag;
int worker_three_flag;

/**
 * Shared ressource used by our threads.
 */
int counter;

void * worker_one();
void * worker_two();
void * worker_three();

int main(int argv, const char ** argc) {
    counter = 0;

    worker_two_flag = 0;
    worker_three_flag = 0;

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);
    
    pthread_create(&threads[0], NULL, worker_one, NULL);
    pthread_create(&threads[1], NULL, worker_two, NULL);
    pthread_create(&threads[2], NULL, worker_three, NULL);
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);

    printf("Value started at %d and ends with %d.\n", 0, counter);

    return 0;
}

/**
 * Increases counter by one until it is at 10000.
 */
void * worker_one() {
    printf("Worker one started.\n");
    
    pthread_mutex_lock(&mutex);
    
    printf("Worker one starting work.\n");

    while (counter < 10000) {
        counter += 7;
    }

    worker_two_flag = 1;

    pthread_cond_broadcast(&cond);
       
    printf("Worker one finished work with: %d.\n", counter);

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

/**
 * Decreases counter until it is at 0xfffff.
 */
void * worker_two() {
    printf("Worker two started.\n");
    
    pthread_mutex_lock(&mutex);
    
    while (!worker_two_flag) {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("Worker two starting work.\n");

    while (counter > 1000)
        counter -= 3;

    printf("Worker two finished work with: %d.\n", counter);

    worker_three_flag = 1;

    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

/**
 * Divides counter by 0xf.
 */
void * worker_three() {
    printf("Worker three started.\n");
    
    pthread_mutex_lock(&mutex);

    while (!worker_three_flag) {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("Worker three starting work.\n");

    counter /= 9;

    printf("Worker three finished work with: %d.\n", counter);

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}
