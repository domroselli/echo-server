#include "../inc/client.h"
#include "../inc/util.h"

void print_usage( ) {
    fprintf( stderr, "usage: client hostname portnumber\n" );
    exit( EXIT_FAILURE );
}

// get sockaddr, IPv4 or IPv6
void *get_in_addr( struct sockaddr *sa ) {
    if ( sa->sa_family == AF_INET ) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void client_init_hints( struct addrinfo *hints ) {
    memset( hints, 0, sizeof( *hints ) );
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
}

int connection_attempt(  const char *hostname, const char *port ) {
    struct addrinfo hints;
    struct addrinfo *p         = NULL;
    struct addrinfo *pservinfo = NULL;
    int             sockfd;
    int             result;
    char            dst[INET6_ADDRSTRLEN] = { '\0' };
     
    client_init_hints( &hints );
    result = getaddrinfo( hostname, port, &hints, &pservinfo );
    if ( result != GETADDRINFO_SUCCESS ) {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( result ) );
        return CONNECTION_ATTEMPT_FAILURE;
     }

    for ( p = pservinfo; p != NULL; p = p->ai_next ) {
        sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
        if ( sockfd == SOCKET_FAILURE ) {
            perror( "client: socket" );
            continue;
        }

        result = connect( sockfd, p->ai_addr, p->ai_addrlen );
        if ( result == CONNECTION_FAILURE ) {
            close( sockfd );
            perror( "client: connect" );
            continue;
        }

        //
        // success!
        //
        break;
    }

    if ( p == NULL ) {
        return CONNECTION_ATTEMPT_FAILURE; 
    } 

    inet_ntop(p->ai_family,
              get_in_addr( (struct sockaddr *)p->ai_addr ), 
              dst, 
              sizeof( dst ) );

    printf( "client: connecting to %s\n", dst);
    freeaddrinfo( pservinfo );

     return sockfd;
}

int process_input( char *buf, size_t bufsize ) {
    int inbytes = 0;

    printf("\n> ");
    inbytes = getline(&buf, &bufsize, stdin );

    // remove the '\n' char
    buf[inbytes-1] = '\0';
    if ( strcmp( buf, "quit" ) == 0 ) {
        printf( "Goodbye!\n" );
        return QUIT;
    }

    return inbytes;
    /*
    if (sbuf[inbytes-1] == '\n') {
        sbuf[inbytes-1] = '\0';
    }
    */
}

int main( int argc, char *argv[] ) {
    char const      *hostname  = NULL;
    char const      *port      = NULL;
    int             sockfd     = -1;
    int             rbytes     =  0;
    int             inbytes    =  0;
    char            *buf       = NULL;
    
    if ( argc < MAXARGS ) {
        print_usage( );
        exit( EXIT_FAILURE );
    }
    
    hostname = argv[1];
    port = argv[2];

    buf = (char *)malloc( sizeof( char ) * BUFSIZE );
    memset( buf, 0, sizeof( char ) * BUFSIZE );

    sockfd = connection_attempt( hostname, port );
    if ( sockfd == CONNECTION_ATTEMPT_FAILURE ) {
        fprintf( stderr, "client: failed to connect\n" );
         exit( EXIT_FAILURE );
    }

    printf( "Type characters at the prompt and they will be echoed back\n" );
    printf( "Type 'quit' to quit the session.\n" );
    while ( ( inbytes = ( process_input( buf, BUFSIZE ) > 0 ) ) ) {
        connection_send( sockfd, buf, strlen( buf ) );
        
        rbytes = connection_recv( sockfd, buf, BUFSIZE );
        if ( rbytes == CONNECTION_RECV_FAILURE ) {
            exit( EXIT_FAILURE );
        }
    }

    close( sockfd );
    free( buf );

    return 0;
}
