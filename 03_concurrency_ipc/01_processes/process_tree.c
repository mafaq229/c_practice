/*
 * CS-6200 Preparation - Module 01: Process Tree
 *
 * Create structured process hierarchies.
 * Understanding process relationships is key to debugging.
 *
 * Compile: clang -Wall -Wextra -g process_tree.c -o process_tree
 * Run:     ./process_tree
 *
 * Difficulty: [MEDIUM]
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* ============================================================================
 * EXERCISE 1: Create a chain of processes
 * ============================================================================
 *
 * Create: Parent -> Child -> Grandchild -> Great-grandchild
 */

void exercise1_chain(int depth) {
    printf("\n=== Exercise 1: Process Chain (depth=%d) ===\n", depth);

    /*
     * TODO: Create a chain of processes where each process
     * creates exactly one child.
     *
     * Expected output (depth=3):
     *   Process 0: PID=1234
     *   Process 1: PID=1235, Parent=1234
     *   Process 2: PID=1236, Parent=1235
     *   Process 3: PID=1237, Parent=1236
     */

    for (int i = 0; i <= depth; i++) {
        printf("Process %d: PID=%d", i, getpid());
        if (i > 0) {
            printf(", Parent=%d", getppid());
        }
        printf("\n");

        if (i < depth) {
            pid_t pid = fork();

            if (pid < 0) {
                perror("fork");
                exit(1);
            } else if (pid > 0) {
                /* Parent waits for child and exits */
                wait(NULL);
                exit(0);
            }
            /* Child continues loop */
        }
    }
}

/* ============================================================================
 * EXERCISE 2: Create a binary tree of processes
 * ============================================================================
 *
 *           Root
 *          /    \
 *      Left      Right
 *      /  \      /   \
 *    LL   LR   RL    RR
 */

void exercise2_binary_tree(int depth, const char *name) {
    printf("Process '%s': PID=%d, Parent=%d\n", name, getpid(), getppid());

    if (depth <= 0) {
        return;
    }

    /*
     * TODO: Each process creates two children.
     * Left child gets name + "L", right child gets name + "R"
     */

    /* Create left child */
    pid_t left_pid = fork();

    if (left_pid < 0) {
        perror("fork");
        exit(1);
    } else if (left_pid == 0) {
        /* Left child */
        char new_name[64];
        snprintf(new_name, sizeof(new_name), "%sL", name);
        exercise2_binary_tree(depth - 1, new_name);
        exit(0);
    }

    /* Create right child */
    pid_t right_pid = fork();

    if (right_pid < 0) {
        perror("fork");
        wait(NULL);  /* Wait for left child */
        exit(1);
    } else if (right_pid == 0) {
        /* Right child */
        char new_name[64];
        snprintf(new_name, sizeof(new_name), "%sR", name);
        exercise2_binary_tree(depth - 1, new_name);
        exit(0);
    }

    /* Parent waits for both children */
    wait(NULL);
    wait(NULL);
}

/* ============================================================================
 * EXERCISE 3: Fan-out pattern
 * ============================================================================
 *
 * One parent creates N children (all siblings)
 *
 *         Parent
 *       / | | | \
 *      C1 C2 C3 C4 C5
 */

void exercise3_fan_out(int n) {
    printf("\n=== Exercise 3: Fan-out (%d children) ===\n", n);

    printf("Parent: PID=%d\n", getpid());

    /*
     * TODO: Create N children, all siblings.
     * Each child should print its number and then exit.
     * Parent should wait for all children.
     */

    pid_t children[n];

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            /* Wait for already created children before exiting */
            for (int j = 0; j < i; j++) {
                wait(NULL);
            }
            exit(1);
        } else if (pid == 0) {
            /* Child */
            printf("  Child %d: PID=%d, Parent=%d\n", i, getpid(), getppid());
            usleep(100000 * i);  /* Stagger exits */
            exit(i);
        }
        children[i] = pid;
    }

    /* Parent waits for all children */
    printf("Parent: Waiting for %d children...\n", n);

    for (int i = 0; i < n; i++) {
        int status;
        pid_t finished = wait(&status);
        if (WIFEXITED(status)) {
            printf("Parent: Child PID %d exited with code %d\n",
                   finished, WEXITSTATUS(status));
        }
    }

    printf("Parent: All children done\n");
}

/* ============================================================================
 * EXERCISE 4: Pipeline pattern
 * ============================================================================
 *
 * Simulate a shell pipeline: cmd1 | cmd2 | cmd3
 * Each process reads from previous and writes to next.
 */

void exercise4_pipeline_demo(void) {
    printf("\n=== Exercise 4: Pipeline Pattern Demo ===\n");

    /*
     * This demonstrates the concept of pipelines.
     * In a real implementation, you'd use pipe() to connect processes.
     *
     * Here we just show the process structure.
     */

    int stages = 3;
    printf("Creating %d-stage pipeline\n", stages);

    for (int i = 0; i < stages; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            /* This process is stage i */
            printf("Stage %d: PID=%d\n", i, getpid());
            sleep(1);
            exit(i);
        }
        /* Parent continues to create next stage */
    }

    /* Wait for all stages */
    for (int i = 0; i < stages; i++) {
        wait(NULL);
    }

    printf("Pipeline complete\n");
}

/* ============================================================================
 * EXERCISE 5: Process info collector
 * ============================================================================
 */

void print_process_info(void) {
    printf("\n--- Process Information ---\n");
    printf("PID:    %d\n", getpid());
    printf("PPID:   %d\n", getppid());
    printf("UID:    %d\n", getuid());
    printf("GID:    %d\n", getgid());
    printf("PGID:   %d\n", getpgrp());

    /* Get current working directory */
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("CWD:    %s\n", cwd);
    }

    printf("---------------------------\n");
}

void exercise5_process_info(void) {
    printf("\n=== Exercise 5: Process Information ===\n");

    print_process_info();

    pid_t pid = fork();

    if (pid == 0) {
        printf("\nChild process info:\n");
        print_process_info();
        exit(0);
    }

    wait(NULL);
}

/* ============================================================================
 * EXERCISE 6: Process group demo
 * ============================================================================
 */

void exercise6_process_group(void) {
    printf("\n=== Exercise 6: Process Groups ===\n");

    /*
     * All children inherit the parent's process group.
     * setpgid() can be used to change process group.
     *
     * Process groups are important for:
     * - Sending signals to multiple processes
     * - Job control in shells
     */

    printf("Parent: PID=%d, PGID=%d\n", getpid(), getpgrp());

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            printf("Child %d: PID=%d, PGID=%d (same as parent)\n",
                   i, getpid(), getpgrp());
            sleep(1);
            exit(0);
        }
    }

    /* Wait for children */
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    printf("All processes were in the same process group\n");
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 01: Process Tree\n");
    printf("================================================\n");

    if (argc > 1) {
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1: exercise1_chain(3); break;
            case 2:
                printf("\n=== Exercise 2: Binary Tree (depth=2) ===\n");
                exercise2_binary_tree(2, "R");
                break;
            case 3: exercise3_fan_out(5); break;
            case 4: exercise4_pipeline_demo(); break;
            case 5: exercise5_process_info(); break;
            case 6: exercise6_process_group(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        exercise1_chain(2);
        exercise3_fan_out(3);
        exercise5_process_info();
    }

    printf("\n================================================\n");
    printf("  Process Tree Complete!\n");
    printf("================================================\n\n");

    return 0;
}
