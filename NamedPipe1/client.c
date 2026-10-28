#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO "/tmp/echo_fifo"
int main(void) {
    char messaggio[1024];

    printf("Inserisci un messaggio: ");

    if (fgets(messaggio, sizeof(messaggio), stdin) == NULL)
        return 1;

    int fd = open(FIFO, O_WRONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (write(fd, messaggio, strlen(messaggio)) == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}