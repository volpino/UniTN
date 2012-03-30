#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

#define BUFFER_SIZE 100
#define LISTENQ (1024)

int sock, cli_sock;

void term_handler() {
    printf("Exiting...\n");
    close(cli_sock);
    close(sock);
    exit(0);
}

int main(int argc, char **argv, char **envp) {
    char buffer[BUFFER_SIZE];

    char *port = NULL;
    int server_port;
    struct sockaddr_in server_address;
    int buf_len;

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
        printf("Error while creating socket\n");
        exit(1);
    }

    if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        printf("Error on binding\n");
        exit(1);
    }

    if (listen(sock, LISTENQ) < 0) {
        printf("Error on listen\n");
        exit(1);
    }

    printf("Listening...\n");

    while (1) {
        cli_sock = accept(sock, NULL, NULL);
        if (cli_sock < 0) {
            printf("Error on accept\n");
            exit(1);
        }

        printf("Accepted connection!\n");

        while (1) {
            bzero(buffer, BUFFER_SIZE);
            buf_len = read(cli_sock, buffer, BUFFER_SIZE - 1);
            if (buf_len == 0) {
                printf("Client disconnected\n");
                break;
            }

            printf("%i bytes received\n", buf_len);
            printf("%s\n", buffer);

            buf_len = write(cli_sock, buffer, strlen(buffer));
        }
    }

    return 0;
}
