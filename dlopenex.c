#include <err.h>
#include <errno.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SALT_SIZE 8

// Function to generate a random salt
void generateSalt(char *salt, size_t length) {
    const char *salt_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
    size_t char_set_length = strlen(salt_chars);

    for (size_t i = 0; i < length; ++i) {
        salt[i] = salt_chars[rand() % char_set_length];
    }
    salt[length] = '\0';
}

void printCrypt(const char *name, const char *email, const char *message) {
    void *dlhandle;
    char *(*_crypt)(const char *, const char *);
    char *error;
    char salt[20];

    // Open the crypt library dynamically
    dlhandle = dlopen("libcrypt.so", RTLD_LAZY);
    if (!dlhandle) {
        err(EXIT_FAILURE, "%s\n", dlerror());
        /* NOTREACHED */
    }

    // Dynamically load the crypt function
    *(void **) (&_crypt) = dlsym(dlhandle, "crypt");
    if ((error = dlerror()) != NULL)  {
        err(EXIT_FAILURE, "%s\n", error);
        /* NOTREACHED */
    }

    // Generate a random salt with MD5 ($1$)
    strcpy(salt, "$1$");
    generateSalt(salt + 3, SALT_SIZE);

    // Encrypt the message
    char *encrypted_message = _crypt(message, salt);

    // Print the user details and encrypted message
    printf("User Name: %s\n", name);
    printf("Email: %s\n", email);
    printf("Original Message: %s\n", message);
    printf("Salt used: %s\n", salt);
    printf("Encrypted Message: %s\n", encrypted_message);

    // Close the library
    dlclose(dlhandle);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <name> <email> <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Seed the random number generator
    srand((unsigned int) time(NULL));

    // Encrypt the message with user details
    printCrypt(argv[1], argv[2], argv[3]);

    exit(EXIT_SUCCESS);
}
