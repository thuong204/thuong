#include <stdio.h>

// Personal Information
#define NAME "John Doe"
#define ADDRESS "123 Main St, Springfield, USA"
#define EMAIL "john.doe@example.com"
#define PHONE "123-456-7890"
#define AGE 30

// Function Implementations
void ldtest0(const char *str) {
    (void)printf("ldtest0 => %s\n", str);
}

void ldtest1(const char *str) {
    (void)printf("ldtest1 => %s\n", str);
}

void ldtest2(const char *str) {
    (void)printf("ldtest2 => %s\n", str);
}

int main() {
    // Display personal information
    printf("Personal Information:\n");
    printf("Name: %s\n", NAME);
    printf("Address: %s\n", ADDRESS);
    printf("Email: %s\n", EMAIL);
    printf("Phone: %s\n", PHONE);
    printf("Age: %d\n\n", AGE);

    // Call the test functions
    ldtest0("Hello world!");
    ldtest1("Hello world!");
    ldtest2("Hello world!");

    return 0;
}
