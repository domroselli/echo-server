#include "../inc/server.h"
#include "../inc/util.h"

void print_usage( ) {
    fprintf( stderr, "usage: portnumber\n" );
}

// get sockaddr, IPv4 or IPv6
void *get_in_addr( struct sockaddr *sa ) {
    if ( sa->sa_family == AF_INET ) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sigchld_handler( int s ) {
    while ( waitpid( -1, NULL, WNOHANG) > 0 )
        ;
}

void set_sigaction( ) {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = SA_RESTART;

    if ( sigaction( SIGCHLD, &sa, NULL ) == SIGACTION_FAILURE ) {
        perror( "sigaction" );
        exit( EXIT_FAILURE );
    }
}

void server_init_hints( struct addrinfo *hints ) {
    memset( hints, 0, sizeof( *hints ) );
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
}

int socket_open( struct addrinfo *pservinfo ) {
    int yes = 1;
    struct addrinfo *p;
    int sockfd = SOCKET_OPEN_FAILURE;

    for ( p = pservinfo; p != NULL; p = p->ai_next ) {
        if ( ( sockfd = socket( p->ai_family, 
                                p->ai_socktype, 
                                p->ai_protocol ) ) == SOCKET_FAILURE ) {

            perror( "server: socket" );
            continue;
        }

        if ( setsockopt( sockfd,
                         SOL_SOCKET,
                         SO_REUSEADDR,
                         &yes,
                         sizeof( int ) ) == SOCKOPT_FAILURE) {

            perror( "setsockopt" );
            return SOCKET_OPEN_FAILURE;
        }

        if ( bind( sockfd, 
                   p->ai_addr, 
                   p->ai_addrlen ) == BIND_FAILURE ) {

            close( sockfd );
            perror( "server: bind" );
            continue;
        }

        //        
        // SUCCESS! 
        //
        break;
    }

    if ( !p ) {
        fprintf( stderr, "server: failed to bind\n" );
        return SOCKET_OPEN_FAILURE;
    }

    return sockfd;
}

int socket_create( const char *port ) {
    int result;
    int sockfd;
    struct addrinfo hints;
    struct addrinfo *pservinfo  = NULL;

    server_init_hints( &hints );
    result = getaddrinfo( NULL, port, &hints, &pservinfo );
    
    if ( result != GETADDRINFO_SUCCESS ) {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( result ) ) ;
        return SOCKET_CREATE_FAILURE;
    }

    sockfd = socket_open( pservinfo );
    if ( sockfd == SOCKET_OPEN_FAILURE ) {
        return SOCKET_CREATE_FAILURE;
    }

    if ( listen( sockfd, BACKLOG ) == LISTEN_FAILURE ) {
        perror( "listen" );
        return SOCKET_CREATE_FAILURE; 
    }
    
    freeaddrinfo( pservinfo );

    return sockfd;
}

void connection_service( int sockfd, int accptfd ) {
    int rbytes;
    char *buf;

    if ( !fork( ) ) {
        close( sockfd );
        buf = (char *)malloc( sizeof( char ) * BUFSIZE );
        while ( ( rbytes = connection_recv( accptfd, buf, BUFSIZE ) )
                && rbytes != CONNECTION_RECV_FAILURE) {

            connection_send( accptfd, buf, strlen( buf ) );
        }

        if ( rbytes == CONNECTION_RECV_FAILURE ) {
            exit( EXIT_FAILURE );
        }

        close( accptfd );

        free( buf );
        exit( EXIT_SUCCESS );
    }
}

void connection_wait( int sockfd ) {
    struct sockaddr_storage client_addr;
    socklen_t sin_size              =  0;
    int       accptfd               = -1;
    char      dst[INET6_ADDRSTRLEN] = { '\0' };


    for (;;) {
        sin_size = sizeof( client_addr );
        accptfd = accept( sockfd, (struct sockaddr *)&client_addr, &sin_size );
        if ( accptfd == ACCEPT_FAILURE ) {
            perror( "accept" );
            continue;
        }

        inet_ntop( client_addr.ss_family, 
                   get_in_addr( (struct sockaddr *) &client_addr ), 
                   dst, 
                   sizeof( dst ) );

        printf("server: got connection from %s\n", dst);
        connection_service( sockfd, accptfd );
    }
    close( accptfd );
}

int main( int argc, char *argv[] ) {
    int        sockfd = -1;
    const char *port;

    if (argc < MAXARGS) {
        print_usage();
        exit( EXIT_FAILURE );
    }

    port = argv[1];
    sockfd = socket_create( port );
    if ( sockfd == SOCKET_CREATE_FAILURE ) {
        exit( EXIT_FAILURE );
    }

    // Kill all zombie process
    set_sigaction();

    printf( "server: waiting for connections...\n" );
    connection_wait( sockfd );

    return 0;
}
