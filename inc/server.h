#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H
#include "util.h"
#define MAXARGS 2 

void sigchld_handler(int s);
void print_usage();
int set_sigaction();
void *get_in_addr(struct sockaddr *sa);
void server_set_hints(struct addrinfo *sa);
int socket_open(struct addrinfo *pservinfo);
int socket_create(const char *port);
int incoming_socket_serv(int sockfd, int accptfd);
void incoming_socket_wait(int sockfd);
#endif
