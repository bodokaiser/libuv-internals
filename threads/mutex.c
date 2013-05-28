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

/**
 * Mutex lock object.
 *
 * The mutex lock object ensures that two threads do not get in
 * conflict when accessing the same ressource. An example herefor
 * would be a log file which two threads want to write some lines
 * into. If there would be no mutex lock the lines written to the
 * files would be unordered and the log file not readable.
 */
pthread_mutex_t mutex;

/**
 * Offset counter so the writers now where to start writting to
 * message.
 */
int offset = 0;

/**
 * The message parts.
 */
char hello[5] = "hello";
char world[6] = " world";

/**
 * This is our shared ressource.
 * It is a char array on which each array should write something.
 */
char message[11];

void * write_hello();
void * write_world();

int main(int argv, const char ** argc) {
    pthread_mutex_init(&mutex, NULL);
    
    pthread_create(&threads[0], NULL, write_world, NULL);
    pthread_create(&threads[1], NULL, write_hello, NULL);
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    printf("Message finally written from threads: %s.\n", message);

    return 0;
}

/**
 * This function will copy "hello" to the shared message.
 *
 * Because we do not want the second thread write something
 * into message while we do we lock the mutex. While this
 * time the other thread is in blocking state waiting on 
 * a freed lock.
 */
void * write_hello() {
    printf("HELLO writer started.\n");
    
    pthread_mutex_lock(&mutex);

    sleep(1);

    int i;
    for (i = 0; i < sizeof(hello); i++)
        message[offset + i] = hello[i];

    offset += i;

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

/**
 * This function will copy " world" to the shared message.
 *
 * Actually it should be the first writer which writes 
 * something to message but because we let it sleep
 * we see the full power of mutex. While this thread sleeps
 * the write hello thread is spawned.
 */
void * write_world() {
    printf("WORLD writer started.\n");

    sleep(1);

    pthread_mutex_lock(&mutex);

    int i;
    for (i = 0; i < sizeof(world); i++)
        message[offset + i] = world[i];

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}
