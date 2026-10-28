#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define FIFO "/tmp/echo_fifo"

int main(void) {
    char messaggio[1024];

    signal(SIGPIPE, SIG_IGN);

    int fd = open(FIFO, O_WRONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    while (1) {
        printf("Messaggio (QUIT per uscire): ");

        if (fgets(messaggio, sizeof(messaggio), stdin) == NULL)
            break;

        size_t len = strlen(messaggio);
        ssize_t n = write(fd, messaggio, len);

        if (n == -1) {
            if (errno == EPIPE)
                fprintf(stderr, "Il server non e' disponibile.\n");
            else
                perror("write");

            close(fd);
            return 1;
        }

        if (strcmp(messaggio, "QUIT\n") == 0 ||
            strcmp(messaggio, "QUIT") == 0)
            break;
    }

    close(fd);
    return 0;
}