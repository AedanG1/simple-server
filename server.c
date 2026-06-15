// a simple web server
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

/*
  * 1. get address info for setting up bind
  * 2. establish socket with kernel
  * 3. bind socket to port using address info
  * 4. change socket to listening socket
  * 5. accept incoming connection
  * 6. receive bytes
*/

#define PORT "4321"
#define BACKLOG 20

int main(void) {
  int status, sockfd, client_connection_fd;
  int yes = 1;
  struct addrinfo hints = {0};
  struct addrinfo *server_info = NULL;
  struct addrinfo *cur = NULL;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size;

  // get server address info for bind()
  if ((status = getaddrinfo(NULL, PORT, &hints, &server_info) != 0)) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }

  for (cur = server_info; cur != NULL; cur = cur->ai_next) {
    // establish a socket with the first result that works
    if ((sockfd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    // set the option to allow port reuse
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("server: sockopt");
      exit(1);
    };

    // bind to the port specified in getaddrinfo()
    if (bind(sockfd, cur->ai_addr, cur->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }
  }

  // server_info no longer needed once sockfd is obtained
  freeaddrinfo(server_info);

  // change socket state to listening
  if (listen(sockfd, BACKLOG) == -1) {
    perror("server: listen");
    exit(1);
  }

  printf("Server waiting for connections...\n");

  // accept incoming connections that have completed handshake
  while (1) {
    client_addr_size = sizeof(client_addr);
    if ((client_connection_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_size)) == -1) {
      perror("server: accept"); 
      continue;
    }
    if (send(client_connection_fd, "You've successfully connected to 127.0.0.1:4321!", 48, 0) == -1) {
      perror("send");
      continue;
    }
    close(client_connection_fd);
  }

  return 0;
}
