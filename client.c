// simple client
/*
  * 1. get address info of server needed for socket \o/
  * 2. establish a socket with the kernel \o/
  * 3. establish a connection with the server
  * 4. recv bytes from server
*/
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define ADDR "127.0.0.1"
#define PORT "4321"
#define MAXDATASIZE 100

int main(void) {
    int status, sockfd, numbytes;
    struct addrinfo hints = {0};
    struct addrinfo *server_info = NULL;
    struct addrinfo *cur = NULL;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    char buf[MAXDATASIZE];

    if ((status = getaddrinfo(ADDR, PORT, &hints, &server_info) != 0)) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    for (cur = server_info; cur != NULL; cur = cur->ai_next) {
        // connect to first address possible
        if ((sockfd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, cur->ai_addr, cur->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (cur == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    freeaddrinfo(server_info);

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n", buf);

    close(sockfd);

    return 0;
}
