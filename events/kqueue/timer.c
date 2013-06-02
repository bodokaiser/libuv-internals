#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/event.h>

int kq;

int nev;

int pid;

struct kevent event;
struct kevent change;

int main(void) {
    kq = kqueue();

    if (kq == -1) {
        perror("Error on creating kqueue.\n");

        exit(EXIT_FAILURE);
    }

    EV_SET(&change, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, 5, 0);

    for (;;) {
        nev = kevent(kq, &change, 1, &event, 1, NULL);

        if (nev < 0) {
            perror("Error on receiving event.\n");

            exit(EXIT_FAILURE);
        }

        if (nev > 0) {
            if (event.flags & EV_ERROR) {
                fprintf(stderr, "Event error: %s.\n", strerror(event.data));
            
                exit(EXIT_FAILURE);
            }

            execlp("date", "date", (char *) 0);
        }
    }

    close(kq);

    return EXIT_SUCCESS;
}
