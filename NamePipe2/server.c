#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define FIFO "/tmp/echo_fifo"

int main(void) {
    if (mkfifo(FIFO, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    int fd = open(FIFO, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    FILE *stream = fdopen(fd, "r");
    if (stream == NULL) {
        perror("fdopen");
        close(fd);
        return 1;
    }

    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "QUIT") == 0) {
            printf("[SERVER] Connessione terminata.\n");
            break;
        }

        printf("[SERVER] %s\n", buffer);
        fflush(stdout);
    }

    fclose(stream);
    unlink(FIFO);

    return 0;
}