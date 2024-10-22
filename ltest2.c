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
    (void)printf("ldtest1 => %s\n", str);
}

// Another implementation
void ldtest2(const char *str) {
    (void)printf("ldtest2 => %s\n", str);
}

// Function to decide which implementation to use
void runTest(const char *str, int testCase) {
    switch (testCase) {
        case 0:
            printf("Using default implementation...\n");
            ldtest0(str);
            break;
        case 1:
            printf("Using alternate implementation...\n");
            ldtest1(str);
            break;
        case 2:
            printf("Using another implementation...\n");
            ldtest2(str);
            break;
        default:
            printf("Invalid test case.\n");
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

    // Scenario: another function test
    printf("\nScenario 3: Another function test\n");
    runTest(message, 2); // Calls ldtest2

    return 0;
}
