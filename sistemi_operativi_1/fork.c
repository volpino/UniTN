#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    char *f_flag = NULL;
    char *g_flag = NULL;
    int f, g;
    int i, j;
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "f:g:")) != -1) {
        switch(c) {
            case 'f':
                f_flag = optarg;
                break;
            case 'g':
                g_flag = optarg;
                break;
            default:
                break;
        }
    }
    if (f_flag == NULL || g_flag == NULL) {
        printf("Error: Wrong parametrs!\n");
        exit(1);
    }
    f = atoi(f_flag);
    g = atoi(g_flag);

    printf("Sono il padre generatore di tutto: %d\n", getpid());

    i = 0;  // f counter
    j = 0;  // g counter
    while (i < f && j < g) {
        i++;
        if (fork() == 0) {
            j++;
            i = 0;
            printf("Io sono %d della generazione %d, figlio di %d\n",
                   getpid(), j, getppid());
        }
        wait(NULL);
    }
}
