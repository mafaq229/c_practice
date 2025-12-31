/*
CS-6200 Preparation - Module 01: Exec Examples

Learn how to replace the current process image with exec().
Typically used with fork() to run other programs.

Compile: clang -Wall -Wextra -g exec_example.c -o exec_example
Run:     ./exec_example

Difficulty: [MEDIUM]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* ============================================================================
CONCEPT: exec() family
============================================================================

exec() replaces the current process image with a new program.
After exec(), the original program code is gone!

The exec family:
  execl()  - list of arguments
  execlp() - list, searches PATH
  execle() - list, custom environment
  execv()  - array (vector) of arguments
  execvp() - vector, searches PATH
  execve() - vector, custom environment (the base syscall)

Naming convention:
  l = list of args (arg1, arg2, ..., NULL)
  v = vector of args (char *argv[])
  p = search PATH for executable
  e = custom environment

Common pattern: fork() then exec() in child
*/

/* ============================================================================
EXERCISE 1: Basic exec
============================================================================
*/

void exercise1_basic_exec(void) {
    printf("\n=== Exercise 1: Basic Exec ===\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        /* Child process */
        printf("Child: About to exec 'ls -la'\n");

        /*
         * TODO: Use execlp to run "ls -la"
         *
         * execlp(file, arg0, arg1, ..., NULL)
         * - file: name of program (searches PATH)
         * - arg0: conventionally the program name
         * - arg1, arg2, ...: arguments
         * - NULL: terminates the argument list
         */

        execlp("ls", "ls", "-la", NULL);

        /* If exec returns, it failed */
        perror("execlp");
        exit(1);
    } else {
        /* Parent process */
        wait(NULL);
        printf("Parent: Child finished executing ls\n");
    }
}

/* ============================================================================
EXERCISE 2: exec with execv (vector)
============================================================================
*/

void exercise2_execv(void) {
    printf("\n=== Exercise 2: execv with Vector ===\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        /* Child process */

        /*
         * TODO: Use execvp with an argument array
         *
         * This is often more flexible than execl because you can
         * build the argument array dynamically.
         */

        char *args[] = {"echo", "Hello", "from", "execvp!", NULL};

        execvp("echo", args);

        perror("execvp");
        exit(1);
    } else {
        wait(NULL);
        printf("Parent: execv example done\n");
    }
}

/* ============================================================================
EXERCISE 3: Run a program with arguments
============================================================================
*/

void exercise3_run_program(const char *program, char *const args[]) {
    printf("\n=== Exercise 3: Run Program ===\n");
    printf("Running: %s\n", program);

    /*
     * TODO: Implement a function that runs any program with arguments.
     *
     * Steps:
     * 1. Fork
     * 2. In child: exec the program with args
     * 3. In parent: wait and report exit status
     */

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    } else if (pid == 0) {
        /* Child */
        execvp(program, args);
        perror("execvp");
        exit(127);  /* Standard exit code for "command not found" */
    } else {
        /* Parent */
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("Program exited with code %d\n", exit_code);
        } else if (WIFSIGNALED(status)) {
            printf("Program killed by signal %d\n", WTERMSIG(status));
        }
    }
}

/* ============================================================================
EXERCISE 4: Capture command output
============================================================================
*/

void exercise4_capture_output(void) {
    printf("\n=== Exercise 4: Capture Output (using popen) ===\n");

    /*
     * While this doesn't use exec directly, popen() is a convenient
     * way to run a command and read its output.
     *
     * Internally, popen() does fork + exec + pipe.
     *
     * TODO: Use popen to run "date" and print the output.
     */

    FILE *fp = popen("date", "r");
    if (fp == NULL) {
        perror("popen");
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("Output: %s", buffer);
    }

    int status = pclose(fp);
    printf("Command exit status: %d\n", WEXITSTATUS(status));
}

/* ============================================================================
EXERCISE 5: Build a simple shell command executor
============================================================================
*/

void exercise5_mini_shell(void) {
    printf("\n=== Exercise 5: Mini Shell ===\n");

    /*
     * TODO: Implement a simple command executor.
     *
     * Read commands from the user and execute them.
     * Type "exit" to quit.
     *
     * This is a simplified version of what shells do!
     */

    char line[256];

    printf("Mini shell (type 'exit' to quit)\n");

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        /* Remove newline */
        line[strcspn(line, "\n")] = '\0';

        /* Check for exit */
        if (strcmp(line, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }

        /* Skip empty lines */
        if (strlen(line) == 0) {
            continue;
        }

        /* Parse command into arguments */
        char *args[64];
        int argc = 0;

        char *token = strtok(line, " \t");
        while (token != NULL && argc < 63) {
            args[argc++] = token;
            token = strtok(NULL, " \t");
        }
        args[argc] = NULL;

        if (argc == 0) {
            continue;
        }

        /* Execute command */
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
        } else if (pid == 0) {
            execvp(args[0], args);
            fprintf(stderr, "Command not found: %s\n", args[0]);
            exit(127);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

/* ============================================================================
EXERCISE 6: Environment variables with exec
============================================================================
*/

void exercise6_exec_env(void) {
    printf("\n=== Exercise 6: Exec with Custom Environment ===\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        /*
         * TODO: Use execle or execve to run a command with
         * a custom environment.
         *
         * Run: env (prints environment variables)
         * With custom env: MY_VAR=hello, PATH=/usr/bin
         */

        char *args[] = {"env", NULL};
        char *env[] = {
            "MY_VAR=hello",
            "CUSTOM_PATH=/usr/bin",
            "HOME=/tmp",
            NULL
        };

        execve("/usr/bin/env", args, env);

        perror("execve");
        exit(1);
    } else {
        wait(NULL);
        printf("Parent: Custom environment example done\n");
    }
}

/* ============================================================================
MAIN
============================================================================
*/

int main(int argc, char *argv[]) {
    printf("\n================================================\n");
    printf("  Module 01: Exec Examples\n");
    printf("================================================\n");

    if (argc > 1) {
        int ex = atoi(argv[1]);
        switch (ex) {
            case 1: exercise1_basic_exec(); break;
            case 2: exercise2_execv(); break;
            case 3: {
                char *args[] = {"echo", "Test", "message", NULL};
                exercise3_run_program("echo", args);
                break;
            }
            case 4: exercise4_capture_output(); break;
            case 5: exercise5_mini_shell(); break;
            case 6: exercise6_exec_env(); break;
            default: printf("Unknown exercise: %d\n", ex);
        }
    } else {
        exercise1_basic_exec();
        exercise2_execv();
        exercise4_capture_output();
        exercise6_exec_env();
        /* exercise5_mini_shell() is interactive, run separately */
    }

    printf("\n================================================\n");
    printf("  Exec Examples Complete!\n");
    printf("================================================\n\n");

    return 0;
}
