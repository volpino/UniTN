#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define PROMPT "→ "
#define BUFFER 1000
#define ARGS 100
#define ARG_SIZE 100


int child_pid;


int parse_args(char buffer[], char **args) {
    char *pch;
    int i = 0;
    pch = strtok(buffer,"\n ");
    while (pch != NULL && i < ARGS-1) {
        strncpy(args[i], pch, ARG_SIZE);
        i++;
        pch = strtok(NULL, "\n ");
    }
    args[i] = NULL;
    return i;
}


void term_handler() {
    if (child_pid) {
        printf("figlio!\n");
        kill(child_pid, SIGINT);
        wait(NULL);
    }
    else {
        exit(0);
    }
}


int main(int argc, char **argv, char **envp) {
    char buffer[BUFFER];
    char *args[ARGS];
    int args_c;
    int i;

    struct sigaction sa;
    sa.sa_handler = &term_handler;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    for (i=0; i<ARGS; i++) {
        args[i] = ((char*) malloc(ARG_SIZE * sizeof(char)));
    }

    while (1) {
        printf(PROMPT);
        fgets(buffer, BUFFER, stdin);
        args_c = parse_args(buffer, args);

        if (feof(stdin) || strcmp("exit", args[0]) == 0) {
            exit(0);
        }

        if (args_c == 0) {
            continue;
        }

        child_pid = fork();
        if (child_pid == 0) {
            execvp(args[0], args);
            perror("exec");
            exit(-1);
        }
        wait(NULL);
        child_pid = 0;
    }

    for (i=0; i<ARGS; i++) {
        free(args[i]);
    }

    return 0;
}
