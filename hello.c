#include <stdio.h>

#ifndef FOOD
#define FOOD "Avocado"
#endif

// Personal Information
#define NAME "John Doe"
#define AGE 30
#define EMAIL "john.doe@example.com"

// Function to mask sensitive data (e.g., email)
void maskEmail(char *email) {
    int len = 0;
    while (email[len] != '\0') len++;  // Get email length
    for (int i = 0; i < len - 4; i++) {
        email[i] = '*';  // Mask everything except the last 4 characters
    }
}

void func2(void) {
    printf("%s: great on anything.\n", FOOD);
}

void func1(void) {
    func2();
}

int main() {
    // Display personal information
    char email[] = EMAIL;
    maskEmail(email);  // Mask sensitive part of the email

    printf("User Information:\n");
    printf("Name: %s\n", NAME);
    printf("Age: %d\n", AGE);
    printf("Email: %s\n", email);

    // Custom scenario output
    printf("\nLet's talk about food!\n");
    func1();

    return 0;
}
