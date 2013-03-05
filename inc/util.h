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

/* terminal color codes */
#define KNRM  "\x1B[0;0m"
#define KRED  "\x1B[0;31m"
#define KGRN  "\x1B[0;32m"
#define KYEL  "\x1B[0;33m"
#define KBLU  "\x1B[0;34m"
#define KMAG  "\x1B[0;35m"
#define KCYN  "\x1B[0;36m"
#define KWHT  "\x1B[0;37m"
#define KRST  "\x1B[0m"

/* client-server */
#define BACKLOG    10
#define FAILURE   -1
#define SUCCESS    0
#define BUFSIZE    1000
#define QUIT       0      

int socket_recv(int fd, char *buf, size_t bufsize);
int socket_send(int fd, const char *buf, size_t slen);
/*
int recvn(int fd, char *vptr, size_t n);
int sendn(int fd, const char *vptr, size_t n);
*/
#endif
