#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int numeri[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int fd[2];
    int somma_pari = 0;
    int somma_dispari = 0;
    int somma_totale;
    pid_t pid;


    if (pipe(fd) == -1) {
        perror("Errore nella creazione della pipe");
        return 1;
    }


    pid = fork();

    if (pid < 0) {
        perror("Errore nella fork");
        return 1;
    }

    if (pid == 0) {

        close(fd[0]);

        for (int i = 0; i < 10; i++) {
            if (numeri[i] % 2 == 0) {
                somma_pari += numeri[i];
            }
        }


        if (write(fd[1], &somma_pari,
                  sizeof(somma_pari)) == -1) {
            perror("Errore nella write");
            close(fd[1]);
            exit(1);
                  }

        close(fd[1]);
        exit(0);

    } else {

        close(fd[1]);

        for (int i = 0; i < 10; i++) {
            if (numeri[i] % 2 != 0) {
                somma_dispari += numeri[i];
            }
        }


        if (read(fd[0], &somma_pari,
                 sizeof(somma_pari)) != sizeof(somma_pari)) {
            perror("Errore nella read");
            close(fd[0]);
            wait(NULL);
            return 1;
                 }

        close(fd[0]);


        wait(NULL);

        somma_totale = somma_pari + somma_dispari;

        printf("Somma dei numeri pari: %d\n", somma_pari);
        printf("Somma dei numeri dispari: %d\n", somma_dispari);
        printf("Somma totale: %d\n", somma_totale);
    }

    return 0;
}