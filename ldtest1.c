#include <stdio.h>

// Personal Information
#define NAME "John Doe"
#define ADDRESS "123 Main St, Springfield, USA"
#define EMAIL "john.doe@example.com"
#define PHONE "123-456-7890"
#define AGE 30

// Primary function implementation
void ldtest0(const char *str) {
    (void)printf("ldtest0 => %s\n", str);
}

// Alternate implementation
void ldtest1(const char *str) {
    (void)printf("alternate ldtest1 implementation => %s\n", str);
}

// Function to decide which implementation to use
void runTest(const char *str, int useAlternate) {
    if (useAlternate) {
        printf("Using alternate implementation...\n");
        ldtest1(str);
    } else {
        printf("Using default implementation...\n");
        ldtest0(str);
    }
}

int main() {
    const char *message = "Testing function implementation!";

    // Display personal information
    printf("Personal Information:\n");
    printf("Name: %s\n", NAME);
    printf("Address: %s\n", ADDRESS);
    printf("Email: %s\n", EMAIL);
    printf("Phone: %s\n", PHONE);
    printf("Age: %d\n", AGE);

    // Scenario: default function test
    printf("\nScenario 1: Default function test\n");
    runTest(message, 0); // Calls ldtest0

    // Scenario: alternate function test
    printf("\nScenario 2: Alternate function test\n");
    runTest(message, 1); // Calls ldtest1

    return 0;
}
