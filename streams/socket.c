#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/event.h>
#include <sys/socket.h>

#define HOSTNAME "127.0.0.1"
#define PORT "3000"

/**
 * Used to check return values.
 */
int r;

/**
 * Instance of kqueue.
 */
int kq;

/**
 * Amount of new events.
 */
int nev;

/**
 * Socket file descriptor.
 */
int sockfd;

/**
 * Server socket address.
 */
struct addrinfo * addr;
struct addrinfo hints;

/**
 * Subscribes and published events.
 */
struct kevent events[10];
struct kevent changes[10];

void read_cb();

int main(int argc, const char ** argv) {
    kq = kqueue();

    if (kq == -1) {
        perror("Error on creating kqueue.\n");
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    r = getaddrinfo(HOSTNAME, PORT, &hints, &addr);

    if (r == -1) {
        perror("Error on resolving address.\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (sockfd == -1) {
        perror("Error on creating socket.\n");
        exit(EXIT_FAILURE);
    }

    r = bind(sockfd, addr->ai_addr, addr->ai_addrlen);

    if (r == -1) {
        perror("Error on binding to address.\n");
        exit(EXIT_FAILURE);
    }

    r = listen(sockfd, 10);

    if (r == -1) {
        perror("Error on listening on socket.\n");
        exit(EXIT_FAILURE);
    }

    EV_SET(&changes[0], sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT, 
            0, 0, 0);

    for (;;) {
        nev = kevent(kq, changes, 10, events, 10, NULL);

        if (nev == -1) {
            perror("Error on receiving events.\n");
            exit(EXIT_FAILURE);
        }

        /* handle new events */
        for (int i = 0; i < nev; i++) {
            /* handle server specific events */
            if (events[i].ident == sockfd) {
                printf("handling events\n");

                int fd = accept(sockfd, (struct sockaddr *) &addr, 
                        (socklen_t *) addr->ai_addrlen);

                if (fd == -1) {
                    perror("Error on accepting client.\n");
                    exit(EXIT_FAILURE);
                }

                EV_SET(&changes[1], fd, EVFILT_READ, 
                        EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 1, 0);
            
                printf("Client %d connected.\n", fd);
            }

            /* handle client specific events */
            if (events[i].data == 1) {
                char * buffer = malloc(1024);
                FILE * client = malloc(sizeof(FILE));

                client = fdopen(events[i].ident, "r");

                fseek(client, 0, SEEK_SET);
                fread(buffer, 1024, 1, client);

                printf("%s\n", buffer);
                
                fclose(client);

                free(buffer);
                free(client);
            }
        }
    }

    return EXIT_SUCCESS;
}
