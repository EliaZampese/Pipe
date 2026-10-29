#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (!pid)
        execl("./secondo", "secondo", NULL);
    wait(NULL);
    printf("\nIl figlio ha finito\n");
    return 0;
}