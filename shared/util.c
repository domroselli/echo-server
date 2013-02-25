#include "../inc/util.h"
int connection_recv( int sockfd, char *buf, size_t bufsize ) {
    int rbytes = 0;
    rbytes = recv( sockfd, buf, bufsize-1, 0 );
    if ( rbytes == RECV_FAILURE ) {
        perror( "recv" );
        return CONNECTION_RECV_FAILURE;
    }   

    // get rid of the newline
    /*
    if (buf[rbytes-1] == '\n') {
        buf[rbytes-1] = '\0';
    }
    */
    buf[rbytes] = '\0';
    printf( "received: %s\n", buf );
    
    return rbytes;
}

int connection_send( int sockfd, const char *buf, size_t bufsize) { 
    if ( send( sockfd, buf, bufsize, 0 ) == SEND_FAILURE ) {
        perror( "send " );
        return SEND_FAILURE;
    }

    printf( "send: %s\n", buf );

    return SEND_SUCCESS;
}

