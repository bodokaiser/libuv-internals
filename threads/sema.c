#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sema;

pthread_t threads[2];

pthread_cond_t cond;

pthread_mutex_t mutex;

int value;

void * worker_a();
void * worker_b();

int main() {
    value = 3;

    sem_init(&sema, 0, 1);

    pthread_create(&threads[0], NULL, worker_a, NULL);
    pthread_create(&threads[1], NULL, worker_b, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    pthread_detach(threads[0]);
    pthread_detach(threads[1]);

    sem_destroy(&sema);

    printf("Value has been set to: %d.\n", value);

    return 0;
}

/**
 * Multiplicates value by 4.
 */
void * worker_a() {
    sem_wait(&sema);

    value *= 4;

    sem_post(&sema);

    pthread_exit(NULL);
}

/**
 * Divides value by 2.
 */
void * worker_b() {
    sem_wait(&sema);

    value /= 2;

    sem_post(&sema);

    pthread_exit(NULL);
}
