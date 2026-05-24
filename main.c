// program that lists the IP Addresses of a given hostname
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  int status;
  struct addrinfo hints, *res, *p;
  char ip_string[INET6_ADDRSTRLEN];
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;     // either ipv4 or 6
  hints.ai_socktype = SOCK_STREAM; // TCP

  if (argc != 2) {
    fprintf(stderr, "usage: ./main <hostname to look up>\n");
  }

  // gives *res pointer the address of the head of the list of addrinfo nodes
  if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
  }

  // extract IP Address from each node in res
  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;
    char *ipver;
    struct sockaddr_in *ipv4;
    struct sockaddr_in6 *ipv6;

    // cast struct sockaddr as sockadder_in or _in6
    // so sin_addr or sin6_addr can be accessed
    if (p->ai_family == AF_INET) { // IPv4
      ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    } else { // IPv6
      ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }

    // convert IP Address to string
    inet_ntop(p->ai_family, addr, ip_string, INET6_ADDRSTRLEN);
    printf("IP Address: %s, %s\n", ipver, ip_string);
  }

  freeaddrinfo(res);

  return 0;
}
