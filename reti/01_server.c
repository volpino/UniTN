#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>

#define BUFFER_SIZE 100
#define LISTENQ (1024)
#define QUEUE 5

int sock, cli_sock;

int connectlist[QUEUE];  /* Array of connected sockets so we know who
                        we are talking to */
fd_set socks;        /* Socket file descriptors we want to wake
                        up for, using select() */
int highsock;      /* Highest #'d file descriptor, needed for select() */

void term_handler() {
    printf("Exiting...\n");
    close(cli_sock);
    close(sock);
    exit(EXIT_SUCCESS);
}

void setnonblocking(int sock)
{
    int opts;

    opts = fcntl(sock,F_GETFL);
    if (opts < 0) {
        perror("fcntl(F_GETFL)");
        exit(EXIT_FAILURE);
    }
    opts = (opts | O_NONBLOCK);
    if (fcntl(sock,F_SETFL,opts) < 0) {
        perror("fcntl(F_SETFL)");
        exit(EXIT_FAILURE);
    }
    return;
}

void build_select_list() {
    int i;

    FD_ZERO(&socks);

    FD_SET(sock, &socks);

    for (i=0; i<5; i++) {
        if (connectlist[i] != 0) {
            FD_SET(connectlist[i], &socks);
            if (connectlist[i] > highsock) {
                highsock = connectlist[i];
            }
        }
    }
}


int main(int argc, char **argv, char **envp) {
    char buffer[BUFFER_SIZE];

    char *port = NULL;
    int server_port;
    struct sockaddr_in server_address;
    int buf_len;

    int reuse_addr = 1;  /* Used so we can re-bind to our port
                            while a previous connection is still
                            in TIME_WAIT state. */
    struct timeval timeout;  /* Timeout for select */
    int readsocks;       /* Number of sockets ready for reading */

    int i;

    struct sigaction sa;
    sa.sa_handler = &term_handler;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    int c;
    opterr = 0;

    while ((c = getopt (argc, argv, "p:")) != -1) {
        switch (c) {
            case 'p':
                port = optarg;
                break;
            case '?':
                if (optopt == 'p')
                    fprintf(stderr, "Option -%c requires an argument.\n",
                            optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                break;
        }
    }

    if (port == NULL) {
        printf("Wrong parameters\n");
        exit(1);
    }

    server_port = atoi(port);
    if (!server_port) {
        printf("Incorrect parameters\n");
        exit(1);
    }


    bzero((char * ) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,
               sizeof(reuse_addr));
    setnonblocking(sock);


    if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("bind");
        close(sock);
        exit(1);
    }

    if (listen(sock, LISTENQ) < 0) {
        perror("listen");
        close(sock);
        exit(1);
    }

    highsock = sock;
    bzero(connectlist, QUEUE);

    printf("Listening...\n");

    while (1) {
        build_select_list();
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        readsocks = select(highsock+1, &socks, (fd_set *) 0,
                           (fd_set *) 0, &timeout);

        if (readsocks < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (readsocks == 0) {
            printf(".");
            fflush(stdout);
        }
        else {
            if (FD_ISSET(sock, &socks)) {
                printf("\nAccepting...\n");
                cli_sock = accept(sock, NULL, NULL);
                if (cli_sock < 0) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }

                setnonblocking(cli_sock);

                for (i=0; (i<5) && (cli_sock != -1); i++) {
                    if (connectlist[i] == 0) {
                        printf("\nConnection accepted:   FD=%d; Slot=%d\n",
                               cli_sock, i);
                        connectlist[i] = cli_sock;
                        cli_sock = -1;
                    }
                }

                if (cli_sock != -1) {
                    printf("\nNo room left for new client.\n");
                    strcpy(buffer, "Sorry, this server is too busy.\n");
                    write(cli_sock, buffer, strlen(buffer));
                    close(cli_sock);
                }
            }

            for (i=0; i<5; i++) {
                if (FD_ISSET(connectlist[i], &socks)) {
                    bzero(buffer, BUFFER_SIZE);
                    buf_len = read(connectlist[i], buffer, BUFFER_SIZE - 1);
                    if (buf_len == 0) {
                        printf("\nClient disconnected\n");
                        close(connectlist[i]);
                        connectlist[i] = 0;
                    }
                    else {
                        printf("\n%i bytes received\n", buf_len);
                        printf("%s\n", buffer);

                        buf_len = write(connectlist[i], buffer, strlen(buffer));
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
