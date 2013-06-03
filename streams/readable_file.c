#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/event.h>

#define PATH "some.temp"

int kq;

int fd;

int nev;

int off = 0;

struct kevent event;
struct kevent change;

void read_cb();

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
 * The following code actually only tries to emulate an offset read from a file.
 */
void read_cb() {
    struct stat * fd_stat = (struct stat *) malloc(sizeof(struct stat));

    fstat(fd, fd_stat);

    int size = fd_stat->st_size;
    
    char * buf = (char *) malloc(size);
    char * buf2 = (char *) malloc(off);

    read(fd, buf, size);

    int a = 0;
    for (int i = off; i < (size - off); i++) {
        buf2[a] = buf[i];
        a++;
    } 

    printf("%s\n", buf2);

    off += size;
}
