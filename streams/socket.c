#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/event.h>
#include <sys/types.h>
#include <sys/socket.h>

int kq;

int nev;

int r, i;

int sockfd;

char buffer[1024];

struct kevent events[2];
struct kevent changes[2];

struct client_s {
    int fd;
    int type;
    socklen_t addrlen;
    struct sockaddr addr;
};

int main(int argc, const char ** argv) {
    struct addrinfo * addr = malloc(sizeof(struct addrinfo));
    struct addrinfo * hints = malloc(sizeof(struct addrinfo));
    
    memset(changes, 0, sizeof(changes));
    memset(events, 0, sizeof(events));

    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_protocol = IPPROTO_TCP;

    r = getaddrinfo("127.0.0.1", "3000", hints, &addr);

    if (r == -1) {
        perror("Error on resolving address.\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (sockfd == -1) {
        perror("Error on creating socket descriptor.\n");
        exit(EXIT_FAILURE);
    }
 
    r = bind(sockfd, addr->ai_addr, addr->ai_addrlen);

    if (r == -1) {
        perror("Error on binding to address.\n");
        exit(EXIT_FAILURE);
    }

    r = listen(sockfd, 1);

    if (r == -1) {
        perror("Error on listening on socket.\n");
        exit(EXIT_FAILURE);
    }

    kq = kqueue();

    if (kq == -1) {
        perror("Error on creating kqueue.\n");
        exit(EXIT_FAILURE);
    }
    
    EV_SET(&changes[0], sockfd, EVFILT_READ, 
            EV_ADD | EV_ENABLE, 0, 0, 0);

    for (;;) {
        int c_n = 1;
        if (changes[1].ident) {
            c_n = 2;
        }
        
        nev = kevent(kq, changes, c_n, events, 2, NULL);

        if (nev == -1) {
            perror("Error on resolving kevents.\n");
            exit(EXIT_FAILURE);
        }
        
        printf("Got event\n");
        
        for (i = 0; i < nev; i++) {
            struct client_s * client = malloc(sizeof(struct client_s));
            
            if (events[i].ident == sockfd) {
                printf("Accepting\n");
                client->fd = accept(sockfd, &client->addr, &client->addrlen);

                if (client->fd == -1) {
                    perror("Error on accepting client.\n");
                    exit(EXIT_FAILURE);
                }

                client->type = 2;

                EV_SET(&changes[1], client->fd, EVFILT_READ, 
                        EV_ADD | EV_ENABLE, 0, 0, client);
            }
            
            if (events[i].udata) {
                client = events[i].udata;

                if (client->type == 2) {
                    recv(client->fd, buffer, 1024, 0);
                    printf("client says: %s\n", buffer);
                }
            }
        }
    }

    close(sockfd);

    freeaddrinfo(addr);

    return EXIT_SUCCESS;
}
