#include "../inc/util.h"
int socket_recv(int fd, char *buf, size_t bufsize) {
    int nrecvd = 0;
    buf[nrecvd] = '\0';

    if (buf == NULL || bufsize <= 1 || fd < 0) {
        return FAILURE;
    }

    printf(KRST "recv: ");
    while ((nrecvd = recv( fd, buf, bufsize-1, 0)) > 0) {
        printf("%s", buf);
        if (buf[nrecvd-1] == '\n') {
            break;
        }
    }

    if (nrecvd == 0) {
        printf(KYEL "disconnected\n");
    }

    if (nrecvd == -1) {
        perror(KRED "socket_recv");
        return FAILURE;
    } 
    
    buf[nrecvd] = '\0';

    return nrecvd;
}

int socket_send(int fd, const char *buf, size_t slen) { 
    int nleft, nsent, start;

    if (fd < 0 || buf == NULL || slen < 0) {
        return FAILURE;
    }

    start = nsent = 0;
    nleft = slen;

    printf(KRST "send: ");
    while(nleft > 0 && (nsent = send(fd, buf, slen, 0)) > 0) { 
        printf( "%.*s", start+nsent, buf);
        start += nsent;
        nleft -= nsent;
    }

    if (nsent == -1 || (nsent == 0 && nleft > 0)) {
        perror(KRED "socket_send ");
        return FAILURE;
    }

    return SUCCESS;
}
