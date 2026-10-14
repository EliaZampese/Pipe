#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define DIM 200

struct Messaggio {
    char testo[DIM];
    int vocali;
};

int main(void) {
    int pipe1[2], pipe2[2];
    pid_t pid;
    char testo[DIM];
    struct Messaggio msg;


    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Errore nella creazione delle pipe");
        return 1;
    }

    printf("Inserisci una stringa: ");
    if (fgets(testo, DIM, stdin) == NULL) {
        fprintf(stderr, "Errore nella lettura della stringa\n");
        return 1;
    }


    pid = fork();

    if (pid < 0) {
        perror("Errore nella fork");
        return 1;
    }

    if (pid == 0) {

        close(pipe1[1]);
        close(pipe2[0]);


        if (read(pipe1[0], testo, sizeof(testo))
            != sizeof(testo)) {
            perror("Errore nella read");
            close(pipe1[0]);
            close(pipe2[1]);
            exit(1);
        }

        close(pipe1[0]);


        msg.vocali = 0;

        for (int i = 0; testo[i] != '\0'; i++) {
            unsigned char c = (unsigned char)testo[i];

            if (c == '\n') {
                testo[i] = '\0';
                break;
            }

            testo[i] = (char)toupper(c);

            if (testo[i] == 'A' || testo[i] == 'E' ||
                testo[i] == 'I' || testo[i] == 'O' ||
                testo[i] == 'U') {
                msg.vocali++;
            }
        }

        strcpy(msg.testo, testo);


        if (write(pipe2[1], &msg, sizeof(msg))
            != sizeof(msg)) {
            perror("Errore nella write");
            close(pipe2[1]);
            exit(1);
        }

        close(pipe2[1]);
        exit(0);

    } else {
        close(pipe1[0]);
        close(pipe2[1]);


        if (write(pipe1[1], testo, sizeof(testo))
            != sizeof(testo)) {
            perror("Errore nella write");
            close(pipe1[1]);
            close(pipe2[0]);
            wait(NULL);
            return 1;
        }

        close(pipe1[1]);


        if (read(pipe2[0], &msg, sizeof(msg))
            != sizeof(msg)) {
            perror("Errore nella read");
            close(pipe2[0]);
            wait(NULL);
            return 1;
        }

        close(pipe2[0]);


        wait(NULL);

        printf("\nStringa maiuscola: %s\n", msg.testo);
        printf("Numero di vocali: %d\n", msg.vocali);
    }

    return 0;
}