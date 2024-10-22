#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <crypt.h>

void printCrypt(const char *s) {
    // Choose a salt, for example: "$1$randomSalt$" (MD5 encryption)
    const char *salt = "$1$randomSalt$";
    char *encrypted = crypt(s, salt);

    // Check if encryption was successful
    if (encrypted == NULL) {
        perror("crypt error");
        exit(EXIT_FAILURE);
    }

    printf("Original string: %s\n", s);
    printf("Salt used: %s\n", salt);
    printf("Encrypted string: %s\n", encrypted);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Check if input is too long
    if (strlen(argv[1]) > 128) {
        fprintf(stderr, "Error: Input string too long (max 128 characters).\n");
        exit(EXIT_FAILURE);
    }

    printCrypt(argv[1]);
    exit(EXIT_SUCCESS);
}
