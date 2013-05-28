#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/**
 * Our threads.
 */
pthread_t threads[2];

int r;

/**
 * Sleep for a specific amount of seconds.
 */
void * timer_one(void * value);
void * timer_two(void * value);

int main(int argv, const char ** argc) {
    pthread_create(&threads[0], NULL, timer_one, NULL);
    pthread_create(&threads[1], NULL, timer_two, NULL);
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    return 0;
}

void * timer_one(void * value) {
    printf("Timer one started.\n");

    sleep(5);

    printf("Timer one finished.\n");

    pthread_exit(NULL);
}

void * timer_two(void * value) {
    printf("Timer two started.\n");

    sleep(2);

    printf("Timer two finished.\n");

    pthread_exit(NULL);
}
