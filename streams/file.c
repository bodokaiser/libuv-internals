#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/event.h>

#define PATH "some.tmp"

/**
 * Instance of kqueue.
 */
int kq;

/**
 * File descriptor.
 */
int fd;

/**
 * Amount of new events.
 */
int nev;

/**
 * File offset to read from.
 */
int off = 0;

/**
 * File stream pointer.
 */
FILE * file;

/**
 * Subscribes and published events.
 */
struct kevent event;
struct kevent change;

void read_cb();

/**
 * 1. Run `make file && ./make.o`
 * 2. Open a new terminal tab
 * 3. type `echo "hello world" >> some.tmp`
 * 4. look what the stream received
 */
int main(int argc, const char ** argv) {
    kq = kqueue();

    if (kq == -1) {
        perror("Error on creating kqueue.\n");
        exit(EXIT_FAILURE);
    }

    fd = open(PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        perror("Error on opening temp file.\n");
        exit(EXIT_FAILURE);
    }

    file = fdopen(fd, "r");

    EV_SET(&change, fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_ONESHOT, 
            NOTE_EXTEND, 0, 0);

    for (;;) {
        nev = kevent(kq, &change, 1, &event, 1, NULL);

        if (nev == -1) {
            perror("Error on receiving events.\n");
            exit(EXIT_FAILURE);
        }

        if (nev == 1) {
            read_cb();
        }
    }

    return EXIT_SUCCESS;
}

/**
 * Reads added data from file.
 */
void read_cb() {
    fseek(file, 0, SEEK_END);

    int size = (ftell (file)) - off;

    fseek(file, off, SEEK_SET);

    char * buf = (char *) malloc(size);

    fread(buf, size, 1, file);

    printf("%s\n", buf);

    off += size;

    free(buf);
}
