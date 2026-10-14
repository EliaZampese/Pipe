#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(void) {
    int pipe1[2], pipe2[2];
    pid_t pidB, pidC;
    int numero;


    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Errore nella creazione delle pipe");
        return 1;
    }


    pidB = fork();

    if (pidB < 0) {
        perror("Errore nella fork");
        return 1;
    }

    if (pidB == 0) {

        close(pipe1[1]);
        close(pipe2[0]);

        while (read(pipe1[0], &numero, sizeof(numero))
               == sizeof(numero)) {


            if (numero >= 50) {
                int quadrato = numero * numero;


                if (write(pipe2[1], &quadrato,
                          sizeof(quadrato)) != sizeof(quadrato)) {
                    perror("Errore nella write di B");
                    close(pipe1[0]);
                    close(pipe2[1]);
                    exit(1);
                }
            }
        }

        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }


    pidC = fork();

    if (pidC < 0) {
        perror("Errore nella fork");
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        waitpid(pidB, NULL, 0);
        return 1;
    }

    if (pidC == 0) {

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);

        int quadrato;
        long long somma = 0;
        int conteggio = 0;

        printf("Quadrati ricevuti dal processo B:\n");


        while (read(pipe2[0], &quadrato, sizeof(quadrato))
               == sizeof(quadrato)) {

            printf("%d\n", quadrato);
            fflush(stdout);

            somma += quadrato;
            conteggio++;
        }

        close(pipe2[0]);


        if (conteggio > 0) {
            double media = (double)somma / conteggio;

            printf("Numero di quadrati ricevuti: %d\n", conteggio);
            printf("Somma dei quadrati: %lld\n", somma);
            printf("Media aritmetica: %.2f\n", media);
        } else {
            printf("Nessun numero ha superato il filtro.\n");
        }

        exit(0);
    }


    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe2[1]);

    srand((unsigned int)time(NULL));

    printf("Numeri generati dal processo A:\n");

    for (int i = 0; i < 20; i++) {
        numero = rand() % 100 + 1;

        printf("%d ", numero);


        if (write(pipe1[1], &numero, sizeof(numero))
            != sizeof(numero)) {
            perror("Errore nella write di A");
            break;
        }
    }

    printf("\n");
    fflush(stdout);


    close(pipe1[1]);


    waitpid(pidB, NULL, 0);
    waitpid(pidC, NULL, 0);

    printf("Processi B e C terminati.\n");

    return 0;
}