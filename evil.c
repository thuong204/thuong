#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Function to mask sensitive personal data (e.g., email)
void maskSensitiveInfo(char *info) {
    int len = strlen(info);
    for (int i = 0; i < len - 4; i++) {
        info[i] = '*';  // Mask all characters except the last 4
    }
}

// Custom printf that adds a timestamp, personal info, and custom messages
int printf(const char *fmt, ...) {
    va_list args;

    // Get the current time
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Print the timestamp and custom log
    fprintf(stdout, "Custom Logger Activated!\n");
    fprintf(stdout, "Timestamp: %02d-%02d-%04d %02d:%02d:%02d\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec);

    // Display personal information (example)
    char name[] = "John Doe";
    char email[] = "john.doe@example.com";
    
    // Mask the email for privacy
    maskSensitiveInfo(email);
    fprintf(stdout, "User Name: %s\n", name);
    fprintf(stdout, "User Email: %s\n", email); // Display masked email
    
    fprintf(stdout, "Now continuing with the original printf...\n");

    // Start the variadic argument handling
    va_start(args, fmt);

    // Delegate to the original printf functionality
    vprintf(fmt, args);

    // Clean up variadic argument handling
    va_end(args);

    return strlen(fmt);
}

int main() {
    printf("This is a test message: %d, %s\n", 42, "hello world");
    return 0;
}
