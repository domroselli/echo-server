#include "../inc/util.h"
int socket_recv( int sockfd, char *buf, size_t bufsize ) {
    int rbytes = 0;

    if( (rbytes = recvn( sockfd, buf, bufsize-1 )) == RECV_FAILURE ) {
        perror( KRED "socket_recv" );
        return RECV_FAILURE;
    }

    if ( rbytes ) {
        buf[rbytes] = '\0';
        printf( KRST "recv: %s\n", buf );
    } else {
        printf( KYEL "disconnected\n" );
    }
    
    return rbytes;
}

int socket_send( int sockfd, const char *buf, size_t bufsize) { 
    if ( sendn( sockfd, buf, bufsize ) == SEND_FAILURE ) {
        perror( KRED "socket_send " );
        return SEND_FAILURE;
    }

    printf( KRST "send: %s\n", buf );

    return SEND_SUCCESS;
}

int recvn( int sockfd, char *buf, size_t n ) {
    size_t nleft;
    int nrecvd;
    char *ptr;

    ptr = buf;
    nleft = n;
    while ( nleft > 0 ) {
        if ( (nrecvd = recv( sockfd, ptr, nleft, 0 )) < 0 ) {
            if ( errno == EINTR ) {
                nrecvd = 0;
            } else {
                return -1;
            }
        } else if ( nrecvd == 0 ) {
            break;
        }

        nleft -= nrecvd;
        ptr += nrecvd;

        if ( *(ptr-1) == '\n' ) {
            break;
        }
    }

    return (n - nleft);
}

int sendn( int sockfd, const char *buf, size_t n ) {
    size_t nleft;
    int nsent;
    const char *ptr;

    ptr = buf;
    nleft = n;
    while ( nleft > 0 ) {
        if ( (nsent = send( sockfd, ptr, nleft, 0 )) <= 0 ) {
                if ( nsent < 0 && errno == EINTR ) {
                    nsent = 0;
                } else {
                    return -1;
                }
        }

        nleft -= nsent;
        ptr += nsent;
    }

    return n;
}

