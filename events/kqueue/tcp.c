#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/event.h>
#include <sys/socket.h>

#define SIZE 1024

int kq;

int nev;

struct kevent events[2];
struct kevent changes[2];

const char buffer[SIZE];

int tcp_open(const char * hostname, int port);
void tcp_send(int socket, const char buffer[], int length);

int main(int argc, const char ** argv) {
    int tty;
    int socket;
    
    if (argc != 3) {
        fprintf(stderr, "Please pass host and port as parameters.\n");
        exit(EXIT_FAILURE);
    }

    kq = kqueue();

    if (kq == -1) {
        perror("Error on creating kqueue.\n");
        exit(EXIT_FAILURE);
    }

    tty = fileno(stdin);

    if (tty == -1) {
        perror("Error on requesting stdin fd id.\n");
        exit(EXIT_FAILURE);
    }

    socket = tcp_open(argv[1], atoi(argv[2]));

    if (socket == -1) {
        perror("Error on opening tcp socket.\n");
        exit(EXIT_FAILURE);
    }   

    EV_SET(&changes[0], tty, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
    EV_SET(&changes[1], socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

    for (;;) {
        nev = kevent(kq, changes, 2, events, 2, NULL);

        if (nev < 0) {
            perror("Error on requesting events.\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nev; i++) {
            if (events[1].flags & EV_EOF)
                exit(EXIT_FAILURE);

            if (events[i].flags & EV_ERROR) {
                fprintf(stderr, "Event error: %s.\n", 
                        strerror(events[i].data));
                exit(EXIT_FAILURE);
            }

            if (events[i].ident == tty) {
                memset((void *) buffer, 0, SIZE);
                fgets((char *) buffer, SIZE, stdin);
                tcp_send(socket, buffer, strlen(buffer));
            }

            if (events[i].ident == socket) {
                memset((void *) buffer, 0, SIZE);
                read(socket, (void*) buffer, SIZE);
                fputs((char *) buffer, stdout);
            }
        }
    }

    close(kq);

    return EXIT_SUCCESS;
}

int tcp_open(const char * hostname, int port) {
    int socketfd;
    
    struct hostent * host;
    struct sockaddr_in addr;

    host = gethostbyname(hostname);

    socketfd = socket(PF_INET, SOCK_STREAM, 0);

    if (socketfd == -1) {
        perror("Error on creating socket.\n");
        exit(EXIT_FAILURE);
    }

    addr.sin_port = htons(port);
    addr.sin_addr = * ((struct in_addr *) host->h_addr);
    addr.sin_family = AF_INET;

    int r = connect(socketfd, (struct sockaddr *) &addr, 
            sizeof(struct sockaddr));

    if (r == -1) {
        perror("Error on connecting to address.\n");
        exit(EXIT_FAILURE);
    }

    return socketfd;
}

void tcp_send(int socketfd, const char buffer[], int length) {
    int offset = 0;
    int bytes_sent = 0;

    do {
        bytes_sent = send(socketfd, buffer + offset, length - offset, 0);

        if (bytes_sent == -1) {
            perror("Error on sending file.\n");
            exit(EXIT_FAILURE);
        }

        offset += bytes_sent;
    } while (bytes_sent > 0);
}
