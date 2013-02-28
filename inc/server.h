#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MAXARGS                2
#define SIGACTION_FAILURE     -1
#define FORK_SUCCESS           0

void sigchld_handler( int s );
void print_usage();
void set_sigaction();
void *get_in_addr( struct sockaddr *sa );
void server_set_hints( struct addrinfo *sa );
int socket_open( struct addrinfo *pservinfo );
int socket_create( const char *port );
void connection_service( int sockfd, int accptfd );
void connection_wait( int sockfd );
#endif
