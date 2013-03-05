#include "../inc/server.h"
#include "../inc/util.h"

void print_usage( ) {
    fprintf( stderr, "usage: portnumber\n" );
}

// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sigchld_handler(int s) {
    while (waitpid( -1, NULL, WNOHANG) > 0)
        ;
}

int set_sigaction( ) {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    return sigaction(SIGCHLD, &sa, NULL);
}

int socket_open(struct addrinfo *pservinfo) {
    int yes =  1;
    int fd  = -1;
    struct addrinfo *p;

    //
    // Bind to the first addrinfo we can and break out
    //
    for (p = pservinfo; p != NULL; p = p->ai_next) {
        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror(KYEL "server: socket");
            continue;
        }

        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror(KYEL "setsockopt");
            continue;
        }

        if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
            perror(KYEL "server: bind");
            continue;
        }

        // SUCCESS! 
        break;
    }

    if (p == NULL) {
        fprintf(stderr, KRED "socket_open: failed to bind\n");
        if (fd != -1) {
            close(fd);
        }
    }

    return fd;
}

int socket_create(const char *port) {
    int result;
    int fd;
    struct addrinfo hints;
    struct addrinfo *pservinfo  = NULL;

    //
    // set up the passive socket to listen on
    //
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    if ((result = getaddrinfo(NULL, port, &hints, &pservinfo)) != 0) {
        fprintf(stderr, KRED "getaddrinfo: %s\n", gai_strerror(result));
        fd = FAILURE;
        goto error;
    }

    if ((fd = socket_open(pservinfo)) == -1) {
        fd = FAILURE;
        goto error;
    }

    if (listen(fd, BACKLOG) == -1) {
        perror(KRED "listen");
        fd = FAILURE;
        goto error;
    }
    
error:
    freeaddrinfo(pservinfo);

    return fd;
}

int incoming_socket_serv(int sfd, int afd) {
    int recvd, pid, status;
    char *buf;

    if (sfd < 0 || afd < 0) {
        return FAILURE;
    }

    pid = fork();
    if (pid == 0) {
        close(sfd);
        buf = (char *)malloc(sizeof(char) * BUFSIZE);
        memset(buf, 0, sizeof(char) * BUFSIZE);

        while ((recvd = socket_recv(afd, buf, BUFSIZE)) > 0) {
            socket_send(afd, buf, strlen(buf));
        }

        close(afd);
        free(buf);
        
        if (recvd == FAILURE) {
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        waitpid(pid, &status, 0);
    } else {
        return FAILURE;
    }

    if (status != EXIT_SUCCESS) {
        return FAILURE;
    }

    return SUCCESS;
}

void incoming_socket_wait(int sfd) {
    struct sockaddr_storage client;
    socklen_t clen                  =  sizeof(client);
    int       afd                   = -1;
    char      dst[INET6_ADDRSTRLEN] = { '\0' };

    for (;;) {
        if ((afd = accept(sfd, (struct sockaddr *)&client, &clen)) == -1) {
            perror(KRED "accept");
            continue;
        }

        inet_ntop( client.ss_family, 
                   get_in_addr( (struct sockaddr *) &client ), 
                   dst, 
                   sizeof(dst));

        printf(KGRN "server: got connection from %s\n", dst);

        if (incoming_socket_serv(sfd, afd) == FAILURE) {
            printf(KRED "server: client process failed\n");
        }
        close(afd);
    }
}

int main(int argc, char *argv[]) {
    int        fd = -1;
    const char *port;

    if (argc < MAXARGS) {
        print_usage();
        goto error;
    }

    port = argv[1];
    if ((fd = socket_create(port)) == FAILURE) {
        perror("socket_create");
        goto error;
    }

    // Kill all zombie process
    if (set_sigaction() == FAILURE) {
        perror("sigaction");
        goto error;
    }

    printf(KGRN "server: waiting for connections...\n");
    incoming_socket_wait(fd);

    close(fd);
    return 0;

error:
    if (fd != -1) {
        close(fd);
    }
    exit(EXIT_FAILURE);
}
