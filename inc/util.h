#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG                      10
#define GETADDRINFO_SUCCESS          0 
#define SOCKET_FAILURE              -1
#define SOCKET_OPEN_FAILURE         -1
#define SOCKET_CREATE_FAILURE       -1
#define CONNECTION_FAILURE          -1
#define CONNECTION_RECV_FAILURE     -1
#define CONNECTION_ATTEMPT_FAILURE  -1
#define SOCKOPT_FAILURE             -1
#define BIND_FAILURE                -1
#define LISTEN_FAILURE              -1
#define ACCEPT_FAILURE              -1
#define RECV_FAILURE                -1
#define SEND_SUCCESS                 0
#define SEND_FAILURE                -1
#define BUFSIZE                      100
#define QUIT                         0      

int connection_recv( int sockfd, char *buf, size_t bufsize );
int connection_send( int sockfd, const char *buf, size_t bufsize );

#endif
