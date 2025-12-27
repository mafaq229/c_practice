/*
 * CS-6200 Preparation - Module 01: Fork Basics
 *
 * Learn how processes are created using fork().
 * Understanding fork() is essential before moving to threads.
 *
 * Compile: clang -Wall -Wextra -g fork_basics.c -o fork_basics
 * Run:     ./fork_basics
 *
 * Difficulty: [MEDIUM]
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* ============================================================================
 * CONCEPT: fork()
 * ============================================================================
 *
 * fork() creates a new process by duplicating the calling process.
 *
 * After fork():
 * - Parent process: fork() returns child's PID (positive number)
 * - Child process:  fork() returns 0
 * - On error:       fork() returns -1 (no child created)
 *
 * The child is an almost exact copy of the parent:
 * - Same code, same variables (copied, not shared!)
 * - Different PID
 * - Different return value from fork()
 */

/* ============================================================================
 * EXERCISE 1: Basic fork
 * ============================================================================
 */

void exercise1_basic_fork(void) {
    printf("\n=== Exercise 1: Basic Fork ===\n");

    printf("Before fork: PID = %d\n", getpid());

    /* TODO: Call fork() and store the result */
    pid_t pid = fork();

    /* TODO: Check return value and print appropriate message
     *
     * If pid < 0:  fork failed, print error
     * If pid == 0: this is the child process
     * If pid > 0:  this is the parent process
     */

    if (pid < 0) {
        /* Fork failed */
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        /* Child process */
        printf("Child: My PID = %d, Parent PID = %d\n", getpid(), getppid());
    } else {
        /* Parent process */
        printf("Parent: My PID = %d, Child PID = %d\n", getpid(), pid);

        /* TODO: Wait for child to complete */
        wait(NULL);
        printf("Parent: Child has finished\n");
    }
}

/* ============================================================================
 * EXERCISE 2: Variables after fork
 * ============================================================================
 */

void exercise2_variables(void) {
    printf("\n=== Exercise 2: Variables After Fork ===\n");

    int shared_var = 100;

    printf("Before fork: shared_var = %d\n", shared_var);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        /* Child process */
        /* TODO: Modify shared_var in child and print it */
        shared_var = 200;
        printf("Child: shared_var = %d (address: %p)\n", shared_var, (void*)&shared_var);
        exit(0);
    } else {
        /* Parent process */
        /* TODO: Wait for child, then print shared_var */
        wait(NULL);
        printf("Parent: shared_var = %d (address: %p)\n", shared_var, (void*)&shared_var);

        /* Question: Is shared_var 100 or 200 in the parent?
         *
         * Answer: 100! Each process has its own copy of variables.
         * The addresses may look the same, but they're in different
         * virtual address spaces.
         */
    }
}

/* ============================================================================
 * EXERCISE 3: Multiple forks
 * ============================================================================
 */

void exercise3_multiple_forks(void) {
    printf("\n=== Exercise 3: Multiple Forks ===\n");

    /*
     * TODO: Predict how many processes will be created by this code.
     * Then run it and verify.
     *
     * Hint: Each fork() doubles the number of processes.
     */

    printf("PID %d: Before any forks\n", getpid());

    fork();  /* First fork */
    printf("PID %d: After first fork\n", getpid());

    fork();  /* Second fork */
    printf("PID %d: After second fork\n", getpid());

    /*
     * Question: How many times is "After second fork" printed?
     *
     * Answer: 4 times!
     *
     * Process tree:
     *           Original (PID X)
     *           /            \
     *     fork1-parent    fork1-child
     *        /    \          /    \
     *   f2-parent f2-child f2-parent f2-child
     */

    /* Wait for all children to prevent zombies */
    while (wait(NULL) > 0);
}

/* ============================================================================
 * EXERCISE 4: Create N child processes
 * ============================================================================
 */

void exercise4_n_children(int n) {
    printf("\n=== Exercise 4: Create %d Children ===\n", n);

    /*
     * TODO: Create exactly N child processes, NOT a tree.
     * Only the original parent should fork.
     *
     * Each child should:
     * 1. Print its index (0, 1, 2, ...)
     * 2. Sleep for 1 second
     * 3. Exit with its index as the exit code
     *
     * The parent should:
     * 1. Wait for all children
     * 2. Print each child's exit status
     */

    printf("Parent (PID %d) creating %d children\n", getpid(), n);

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            /* Child process */
            printf("Child %d: PID = %d\n", i, getpid());
            sleep(1);
            exit(i);  /* Exit with index as status */
        }
        /* Parent continues loop to create more children */
    }

    /* Parent waits for all children */
    int status;
    pid_t child_pid;

    while ((child_pid = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            printf("Child PID %d exited with status %d\n",
                   child_pid, WEXITSTATUS(status));
        }
    }

    printf("Parent: All children done\n");
}

/* ============================================================================
 * EXERCISE 5: Zombie and Orphan Processes
 * ============================================================================
 */

void exercise5_zombie(void) {
    printf("\n=== Exercise 5: Zombie Process ===\n");

    /*
     * A ZOMBIE process is one that has terminated but hasn't been
     * waited for by its parent. It still has an entry in the process
     * table.
     *
     * TODO: Create a zombie process by:
     * 1. Forking
     * 2. Having the child exit immediately
     * 3. Having the parent sleep without calling wait()
     *
     * While the parent sleeps, run `ps aux | grep zombie` in another
     * terminal to see the zombie (will show 'Z' in status).
     */

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        /* Child: exit immediately */
        printf("Child (PID %d): Exiting...\n", getpid());
        exit(0);
    } else {
        /* Parent: don't wait immediately */
        printf("Parent: Child PID = %d\n", pid);
        printf("Parent: Sleeping 5 seconds WITHOUT waiting...\n");
        printf("Run 'ps aux | grep %d' in another terminal to see zombie\n", pid);
        sleep(5);

        /* Now wait to clean up zombie */
        wait(NULL);
        printf("Parent: Called wait(), zombie cleaned up\n");
    }
}

void exercise5_orphan(void) {
    printf("\n=== Exercise 5b: Orphan Process ===\n");

    /*
     * An ORPHAN process is one whose parent has terminated.
     * Orphans are adopted by init (PID 1) or a system process.
     *
     * TODO: Create an orphan process by:
     * 1. Forking
     * 2. Having the parent exit immediately
     * 3. Having the child sleep, then check its parent PID
     */

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        /* Child */
        printf("Child: My PID = %d, Parent PID = %d\n", getpid(), getppid());
        sleep(2);
        printf("Child (after parent exits): My PID = %d, Parent PID = %d\n",
               getpid(), getppid());
        /* Note: Parent PID will now be 1 (init) or similar system process */
        exit(0);
    } else {
        /* Parent exits immediately */
        printf("Parent (PID %d): Exiting, leaving child as orphan\n", getpid());
        exit(0);
    }
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 01: Fork Basics\n");
    printf("================================================\n");

    if (argc > 1) {
        /* Run specific exercise */
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1: exercise1_basic_fork(); break;
            case 2: exercise2_variables(); break;
            case 3: exercise3_multiple_forks(); break;
            case 4: exercise4_n_children(3); break;
            case 5: exercise5_zombie(); break;
            case 6: exercise5_orphan(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        /* Run exercises that don't create weird process states */
        exercise1_basic_fork();
        exercise2_variables();
        /* Uncomment to run:
        exercise3_multiple_forks();
        exercise4_n_children(3);
        exercise5_zombie();
        exercise5_orphan();
        */
    }

    printf("\n================================================\n");
    printf("  Fork Basics Complete!\n");
    printf("================================================\n\n");

    return 0;
}
