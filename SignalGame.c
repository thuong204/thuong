#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#define MAX_SIGNALS 32

// Structure to hold the hit and defense counters
typedef struct {
    int shots;
    int defenses;
} ProcessCounters;

ProcessCounters parentCounters = {0, 0};
ProcessCounters childCounters = {0, 0};

// Function prototypes
void handle_signal(int signo);
void print_counters();
void fork_process();
void kill_process(ProcessCounters* killerCounters, ProcessCounters* killedCounters);
void siginfo_handler(int signo);
void print_menu();
void print_instructions();

int main() {
    srand(time(NULL)); // Seed the random number generator

    // Set up the SIGINFO handler
    if (signal(SIGIO, siginfo_handler) == SIG_ERR) {
        perror("Unable to set SIGINFO handler");
        exit(EXIT_FAILURE);
    }

    // Display menu
    print_menu();

    return 0;
}

// Handle incoming signals
void handle_signal(int signo) {
    if (signo >= 1 && signo <= MAX_SIGNALS) {
        // Determine which process is calling this function
        if (getpid() == getppid()) { // Parent process
            parentCounters.defenses++;
            if (signo == SIGABRT) {
                kill_process(&parentCounters, &childCounters);
            }
        } else { // Child process
            childCounters.defenses++;
            if (signo == SIGABRT) {
                kill_process(&childCounters, &parentCounters);
            }
        }
    }
}

// Print counters for both processes
void print_counters() {
    printf("\n--- Game Status ---\n");
    printf("Parent - Shots: %d, Defenses: %d\n", parentCounters.shots, parentCounters.defenses);
    printf("Child  - Shots: %d, Defenses: %d\n", childCounters.shots, childCounters.defenses);
}

// Fork a new process and run the game logic
void fork_process() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        while (1) {
            sleep(rand() % 5); // Sleep for a random time (0-4 seconds)
            childCounters.shots++;
            int signal = rand() % (MAX_SIGNALS - 1) + 1; // Random signal (1-32)
            printf("Child sending signal %d\n", signal);
            kill(getppid(), signal);
        }
    } else { // Parent process
        while (1) {
            sleep(rand() % 5); // Sleep for a random time (0-4 seconds)
            parentCounters.shots++;
            int signal = rand() % (MAX_SIGNALS - 1) + 1; // Random signal (1-32)
            printf("Parent sending signal %d\n", signal);
            kill(pid, signal);
        }
    }
}

// Kill the other process
void kill_process(ProcessCounters* killerCounters, ProcessCounters* killedCounters) {
    printf("Killing the other process...\n");
    // Increment shots of the killer
    killerCounters->shots++;
    kill(getppid(), SIGKILL); // Terminate the other process
    exit(EXIT_SUCCESS); // Exit after killing the other process
}

// Handle SIGINFO signal
void siginfo_handler(int signo) {
    printf("\nReceived SIGINFO:\n");
    print_counters();
}

// Print the main menu
void print_menu() {
    int choice;
    do {
        printf("\n=== Welcome to SIGGOT (Signals Game of Thrones) ===\n");
        printf("1. Start Game\n");
        printf("2. Instructions\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                fork_process(); // Start the game
                break;
            case 2:
                print_instructions();
                break;
            case 3:
                printf("Exiting the game.\n");
                exit(EXIT_SUCCESS);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 3);
}

// Print game instructions
void print_instructions() {
    printf("\n--- Instructions for SIGGOT ---\n");
    printf("1. The game involves a battle between two processes: Parent and Child.\n");
    printf("2. Each process will randomly send signals (1-32) to the other.\n");
    printf("3. If a process receives a signal, it defends by increasing its defense counter.\n");
    printf("4. If a process receives SIGABRT, it will kill the other process.\n");
    printf("5. You can press Ctrl+T (SIGINFO) to view the current score of each process.\n");
    printf("6. After one process kills the other, it will restart the game by forking again.\n");
    printf("\nEnjoy the game!\n");
}

