#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define BUFFER_SIZE 100


int main(int argc, char **argv, char **envp) {
    char buffer[BUFFER_SIZE];
    int buf_len;

    char *server = NULL;
    char *port = NULL;
    int server_port = 0;
    struct hostent *server_host = NULL;
    struct sockaddr_in server_address;
    int sock;

    int c;
    char chr;

    opterr = 0;

    while ((c = getopt (argc, argv, "s:p:")) != -1) {
        switch (c) {
            case 's':
                server = optarg;
                break;
            case 'p':
                port = optarg;
                break;
            case '?':
                if (optopt == 's' || optopt == 'p')
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

    if (server == NULL || port == NULL) {
        printf("Wrong parameters\n");
        exit(1);
    }

    server_host = gethostbyname(server);
    server_port = atoi(port);
    if (!server_host || !server_port) {
        printf("Incorrect parameters\n");
        exit(1);
    }


    bzero((char * ) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy(server_host->h_addr, (char *) &server_address.sin_addr,
          server_host->h_length);
    server_address.sin_port = htons(server_port);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("connect");
        exit(1);
    }
    printf("Connected to the server\n");

    do {
        printf(">>> ");
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE - 1, stdin);

        if (strcmp(buffer, "quit\n") == 0) {
            printf("Exiting...");
            break;
        }

        buf_len = write(sock, buffer, strlen(buffer));
        printf("%d bytes sended - %s\n", buf_len, buffer);

        bzero(buffer, BUFFER_SIZE);
        c = 0;
        do {
            // read one byte a time and save it in buffer
            if (read(sock, &chr, 1) > 0) {
                buffer[c] = chr;
                c++;
            }
            else {
                break;
            }
        } while (c < (BUFFER_SIZE - 1) && chr != '\n');
        buffer[c] = '\0';
        printf("%lu bytes received - %s\n", strlen(buffer), buffer);
    } while (strlen(buffer) != 0);

    printf("Connection closed from the server\n");
    close(sock);
    return 0;
}
