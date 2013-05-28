#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/**
 * Our thread.
 */
pthread_t thread;

/**
 * Calculates the faculty of a value.
 */
void * faculty(void * value);

/**
 * Return code of pthread subroutines.
 */
int r;

/**
 * Number value passed on execution.
 */
int value;

/**
 * Pointer to the calculated result.
 */
void * result;

/**
 * Takes first argument as value to calculate faculty in new thread.
 */
int main(int argv, const char ** argc) {
    /* convert string argument to integer */
    value = atoi(argc[1]);
    
    /* spawn faculty thread with value as argument */
    r = pthread_create(&thread, NULL, faculty, (void *) value);

    if (r) {
        return printf("Error on creating thread.\n");
    }

    /* wait until the faculty calculation has finished 
     * and assign a pointer to the result */
    r = pthread_join(thread, &result);

    if (r) {
        return printf("Error on joining thread.\n");
    }

    return printf("Faculty of %d equals %d.\n", value, (int) result);
}

void * faculty(void * value) { 
    int result = 1;

    /* decrease counter and multiplicate with result */
    for (int n = (int) value; n > 0; n--)
        result *= n;

    /* exit thread and pass result */
    pthread_exit((void *) result);
}
