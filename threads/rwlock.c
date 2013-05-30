#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_t threads[2];

pthread_cond_t readc;
pthread_cond_t writec;

pthread_rwlock_t rwlock;

int value;

int readf = 0;
int writef = 0;

void * reader();
void * writer();

int main() {
    value = 1;

    pthread_cond_init(&readc, NULL);
    pthread_cond_init(&writec, NULL);

    pthread_rwlock_init(&rwlock, NULL);

    pthread_create(&threads[0], NULL, reader, NULL);
    pthread_create(&threads[1], NULL, writer, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    pthread_rwlock_destroy(&rwlock);

    pthread_cond_destroy(&writec);
    pthread_cond_destroy(&readc);

    return 0;
}

void * reader() {
    for (int i = 0; i < 5; i++) {
        pthread_rwlock_wrlock(&rwlock);

        pthread_rwlock_tryrdlock(&rwlock);
        
        printf("Reader has read: %d from value.\n", value);

        pthread_rwlock_unlock(&rwlock);
    
        sleep(2);
    }

    pthread_exit(NULL);
}

void * writer() {
    for (int i = 0; i < 5; i++) {
        pthread_rwlock_rdlock(&rwlock);

        pthread_rwlock_trywrlock(&rwlock);

        value += 1;

        pthread_rwlock_unlock(&rwlock);

        sleep(i);
    }

    pthread_exit(NULL);
}
