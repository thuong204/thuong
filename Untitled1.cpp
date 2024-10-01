#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef SLEEP
#define SLEEP 5
#endif

#define MSG "SIGQUIT caught.\n"
#define MAX_COUNT 10 // Gi?i h?n s? l?n nh?n t�n hi?u

static int sigquit_count = 0; // Bi?n d?m s? l?n nh?n du?c SIGQUIT

// H�m x? l� t�n hi?u SIGQUIT
static void sig_quit(int signo) {
    (void)signo;
    sigquit_count++;
    (void)write(STDOUT_FILENO, MSG, strlen(MSG));
    printf("S? l?n SIGQUIT nh?n du?c: %d\n", sigquit_count);

    // Ki?m tra xem c� d?t d?n gi?i h?n kh�ng
    if (sigquit_count >= MAX_COUNT) {
        printf("�� d?t d?n gi?i h?n nh?n t�n hi?u, tho�t chuong tr�nh.\n");
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char **argv) {
    (void)argv;

    if (signal(SIGQUIT, sig_quit) == SIG_ERR) {
        err(EXIT_FAILURE, "unable to set SIGQUIT signal handler");
    }

    printf("=> Waiting for a signal (nh?n Ctrl+\\ d? g?i SIGQUIT)...\n");
    
    // V�ng l?p d? gi? chuong tr�nh ch?y v� ch? t�n hi?u
    while (1) {
        pause(); // Ch? t�n hi?u
        printf("=> Ti?p t?c ch? t�n hi?u...\n");
    }

    exit(EXIT_SUCCESS);
}

