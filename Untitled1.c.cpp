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
#define MAX_COUNT 10 // Gi?i h?n s? l?n nh?n tín hi?u

static int sigquit_count = 0; // Bi?n d?m s? l?n nh?n du?c SIGQUIT

// Hàm x? lý tín hi?u SIGQUIT
static void sig_quit(int signo) {
    (void)signo;
    sigquit_count++;
    (void)write(STDOUT_FILENO, MSG, strlen(MSG));
    printf("S? l?n SIGQUIT nh?n du?c: %d\n", sigquit_count);

    // Ki?m tra xem có d?t d?n gi?i h?n không
    if (sigquit_count >= MAX_COUNT) {
        printf("Ðã d?t d?n gi?i h?n nh?n tín hi?u, thoát chuong trình.\n");
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char **argv) {
    (void)argv;

    if (signal(SIGQUIT, sig_quit) == SIG_ERR) {
        err(EXIT_FAILURE, "unable to set SIGQUIT signal handler");
    }

    printf("=> Waiting for a signal (nh?n Ctrl+\\ d? g?i SIGQUIT)...\n");
    
    // Vòng l?p d? gi? chuong trình ch?y và ch? tín hi?u
    while (1) {
        pause(); // Ch? tín hi?u
        printf("=> Ti?p t?c ch? tín hi?u...\n");
    }

    exit(EXIT_SUCCESS);
}

