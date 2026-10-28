#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define FIFO_A_B "/tmp/chat_a_b"
#define FIFO_B_A "/tmp/chat_b_a"

#ifndef RUOLO
#define RUOLO 'A'
#endif

int leggi_riga(int fd, char *buffer, size_t dim) {
    size_t i = 0;
    char c;

    while (i < dim - 1) {
        ssize_t n = read(fd, &c, 1);

        if (n == 0)
            return 0;

        if (n == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }

        if (c == '\n')
            break;

        buffer[i++] = c;
    }

    buffer[i] = '\0';
    return 1;
}

int main(void) {
    signal(SIGPIPE, SIG_IGN);

    const char *uscita =
        RUOLO == 'A' ? FIFO_A_B : FIFO_B_A;

    const char *ingresso =
        RUOLO == 'A' ? FIFO_B_A : FIFO_A_B;

    if (RUOLO == 'A') {
        if (mkfifo(FIFO_A_B, 0666) == -1 &&
            errno != EEXIST) {
            perror("mkfifo");
            return 1;
        }

        if (mkfifo(FIFO_B_A, 0666) == -1 &&
            errno != EEXIST) {
            perror("mkfifo");
            unlink(FIFO_A_B);
            return 1;
        }
    }

    printf("[%c] In attesa di connessione...\n", RUOLO);

    /* A apre in scrittura, B apre in lettura. */
    int fd_out = open(uscita, O_WRONLY);
    if (fd_out == -1) {
        perror("open uscita");
        return 1;
    }

    int fd_in = open(ingresso, O_RDONLY);
    if (fd_in == -1) {
        perror("open ingresso");
        close(fd_out);
        return 1;
    }

    char messaggio[512];
    char risposta[512];

    while (1) {
        printf("[%c] Inserisci messaggio (EXIT per uscire): ",
               RUOLO);
        fflush(stdout);

        if (fgets(messaggio, sizeof(messaggio), stdin) == NULL)
            break;

        size_t len = strlen(messaggio);

        if (write(fd_out, messaggio, len) == -1) {
            perror("write");
            break;
        }

        int n = leggi_riga(fd_in, risposta, sizeof(risposta));

        if (n <= 0) {
            printf("[%c] Connessione chiusa.\n", RUOLO);
            break;
        }

        printf("[%c] Risposta: %s\n", RUOLO, risposta);

        if (strcmp(messaggio, "EXIT\n") == 0 ||
            strcmp(messaggio, "EXIT") == 0 ||
            strcmp(risposta, "EXIT") == 0) {
            break;
        }
    }

    close(fd_out);
    close(fd_in);

    if (RUOLO == 'A') {
        unlink(FIFO_A_B);
        unlink(FIFO_B_A);
    }

    return 0;
}