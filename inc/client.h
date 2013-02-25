#ifndef CLIENT_H
#define CLIENT_H

#define MAXARGS 3
#include "util.h"

void print_usage( );
int process_input( char *buf, size_t bufsize );
void client_init_hints( struct addrinfo *sa );
void *get_in_addr( struct sockaddr *sa );
int connection_attempt( const char *hostname, const char *port );
#endif

