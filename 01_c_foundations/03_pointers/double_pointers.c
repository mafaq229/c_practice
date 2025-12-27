/*
 * CS-6200 Preparation - Module 03: Double Pointers
 *
 * Double pointers (pointer to pointer) are used extensively in GIOS,
 * especially in Project 3. This module builds your understanding.
 *
 * Compile: clang -Wall -Wextra -g double_pointers.c -o double_pointers
 * Run:     ./double_pointers
 *
 * Topics covered:
 *   - What double pointers are
 *   - When and why to use them
 *   - Modifying pointers through functions
 *   - Dynamic 2D arrays
 *   - Common patterns in systems programming
 *
 * Difficulty: [MEDIUM] to [HARD]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * EXERCISE 1: Understanding Double Pointers [MEDIUM]
 * ============================================================================
 *
 * A double pointer is a pointer that points to another pointer.
 * int **dptr points to an int* which points to an int.
 */
void exercise1_double_pointer_basics(void) {
    printf("\n=== Exercise 1: Double Pointer Basics ===\n");

    int x = 42;
    int *ptr = &x;      /* ptr points to x */
    int **dptr = &ptr;  /* dptr points to ptr */

    printf("x = %d\n", x);
    printf("Address of x: %p\n", (void*)&x);
    printf("\n");

    printf("ptr = %p (holds address of x)\n", (void*)ptr);
    printf("*ptr = %d (value at address ptr holds)\n", *ptr);
    printf("Address of ptr: %p\n", (void*)&ptr);
    printf("\n");

    printf("dptr = %p (holds address of ptr)\n", (void*)dptr);
    printf("*dptr = %p (value at dptr, which is ptr)\n", (void*)*dptr);
    printf("**dptr = %d (value at *dptr, which is x)\n", **dptr);
    printf("\n");

    /* Visualization:
     *
     *  dptr -----> ptr -----> x
     *  [addr_ptr]  [addr_x]   [42]
     *
     *  dptr holds the address of ptr
     *  *dptr gives us ptr (the address of x)
     *  **dptr gives us x (the value 42)
     */

    /* TODO: Modify x using the double pointer */
    /* Uncomment and observe: **dptr = 100; */

    printf("After **dptr = 100, x = %d (expected: 100)\n", x);
}

/* ============================================================================
 * EXERCISE 2: Why Double Pointers? - Modifying Pointers [MEDIUM]
 * ============================================================================
 *
 * The main reason for double pointers: modifying a pointer in a function.
 * Without double pointers, you can only modify what the pointer points TO,
 * not the pointer ITSELF.
 */

/*
 * This function DOES NOT work as intended.
 * The caller's pointer is not modified.
 */
void allocate_wrong(int *ptr, int value) {
    ptr = malloc(sizeof(int));  /* Only modifies local copy! */
    if (ptr != NULL) {
        *ptr = value;
    }
    /* When function returns, caller's ptr is still unchanged */
}

/*
 * This function DOES work.
 * We receive a pointer TO the caller's pointer, so we can modify it.
 */
void allocate_correct(int **ptr, int value) {
    *ptr = malloc(sizeof(int));  /* Modifies caller's pointer */
    if (*ptr != NULL) {
        **ptr = value;
    }
}

void exercise2_modifying_pointers(void) {
    printf("\n=== Exercise 2: Modifying Pointers ===\n");

    /* Using the wrong approach */
    int *p1 = NULL;
    allocate_wrong(p1, 42);
    if (p1 == NULL) {
        printf("allocate_wrong: p1 is still NULL (function failed)\n");
    } else {
        printf("allocate_wrong: p1 = %d\n", *p1);
        free(p1);
    }

    /* Using the correct approach */
    int *p2 = NULL;
    allocate_correct(&p2, 42);
    if (p2 == NULL) {
        printf("allocate_correct: p2 is NULL (allocation failed)\n");
    } else {
        printf("allocate_correct: p2 = %d (expected: 42)\n", *p2);
        free(p2);
    }

    /*
     * KEY INSIGHT:
     * - To modify an int in a function, pass int*
     * - To modify an int* in a function, pass int**
     * - To modify an int** in a function, pass int***
     * - And so on...
     */
}

/* ============================================================================
 * EXERCISE 3: Implementing Functions with Double Pointers [MEDIUM]
 * ============================================================================
 */

/*
 * TODO: Implement a function that allocates an array of n integers,
 * initializes all elements to init_value, and returns the array
 * through the double pointer.
 *
 * Returns: 0 on success, -1 on failure (malloc failed)
 */
int create_array(int **arr, int n, int init_value) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate memory for n integers
     * 2. Check if allocation succeeded
     * 3. If yes, store pointer in *arr and initialize values
     * 4. Return 0 on success, -1 on failure
     */
    return -1;  /* TODO: Fix this */
}

/*
 * TODO: Implement a function that frees an array and sets the pointer to NULL.
 * This is a common pattern to prevent use-after-free bugs.
 */
void destroy_array(int **arr) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Free the memory at *arr
     * 2. Set *arr to NULL
     */
}

void exercise3_double_pointer_functions(void) {
    printf("\n=== Exercise 3: Double Pointer Functions ===\n");

    int *arr = NULL;

    /* Test create_array */
    if (create_array(&arr, 5, 10) == 0) {
        printf("Created array with 5 elements, all set to 10:\n");
        printf("[");
        for (int i = 0; i < 5; i++) {
            printf("%d", arr[i]);
            if (i < 4) printf(", ");
        }
        printf("] (expected: [10, 10, 10, 10, 10])\n");

        /* Test destroy_array */
        destroy_array(&arr);
        if (arr == NULL) {
            printf("After destroy_array, arr is NULL (correct!)\n");
        } else {
            printf("ERROR: arr is not NULL after destroy_array\n");
        }
    } else {
        printf("create_array failed\n");
    }
}

/* ============================================================================
 * EXERCISE 4: Dynamic 2D Arrays [HARD]
 * ============================================================================
 *
 * Double pointers are commonly used for dynamic 2D arrays.
 * This is an array of pointers, where each pointer points to a row.
 */

/*
 * TODO: Implement a function to create a 2D array (matrix).
 * The matrix should be an array of pointers, each pointing to a row.
 *
 * Returns: pointer to the 2D array, or NULL on failure
 */
int **create_matrix(int rows, int cols) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate memory for 'rows' pointers (int **)
     * 2. For each row, allocate memory for 'cols' integers
     * 3. Handle allocation failures (free everything and return NULL)
     */
    return NULL;  /* TODO: Fix this */
}

/*
 * TODO: Implement a function to free a 2D array.
 */
void free_matrix(int **matrix, int rows) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Free each row
     * 2. Free the array of pointers
     */
}

/*
 * TODO: Implement a function to initialize a matrix with values.
 * matrix[i][j] = i * cols + j
 */
void init_matrix(int **matrix, int rows, int cols) {
    /* TODO: Implement this function */
}

/*
 * Helper function to print a matrix
 */
void print_matrix(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++) {
            printf("%3d", matrix[i][j]);
            if (j < cols - 1) printf(", ");
        }
        printf("]\n");
    }
}

void exercise4_2d_arrays(void) {
    printf("\n=== Exercise 4: Dynamic 2D Arrays ===\n");

    int rows = 3, cols = 4;
    int **matrix = create_matrix(rows, cols);

    if (matrix != NULL) {
        init_matrix(matrix, rows, cols);
        printf("Created %dx%d matrix:\n", rows, cols);
        print_matrix(matrix, rows, cols);
        printf("\nExpected:\n");
        printf("  [  0,   1,   2,   3]\n");
        printf("  [  4,   5,   6,   7]\n");
        printf("  [  8,   9,  10,  11]\n");

        free_matrix(matrix, rows);
        printf("\nMatrix freed successfully\n");
    } else {
        printf("Failed to create matrix\n");
    }
}

/* ============================================================================
 * EXERCISE 5: Array of Strings [MEDIUM]
 * ============================================================================
 *
 * char** is used for arrays of strings (array of char pointers).
 * This is the type of argv in main(int argc, char **argv).
 */
void exercise5_string_array(void) {
    printf("\n=== Exercise 5: Array of Strings ===\n");

    /* Static initialization */
    const char *names[] = {"Alice", "Bob", "Charlie", "Diana"};
    int count = sizeof(names) / sizeof(names[0]);

    printf("Static string array:\n");
    for (int i = 0; i < count; i++) {
        printf("  names[%d] = %s\n", i, names[i]);
    }

    /* TODO: Create a dynamic array of strings */
    /* Allocate an array of 3 char* pointers */
    char **dynamic_names = NULL;  /* TODO: Allocate this */
    int dynamic_count = 3;

    /* TODO: Allocate and copy strings */
    /* dynamic_names[0] = strdup("Eve"); */
    /* dynamic_names[1] = strdup("Frank"); */
    /* dynamic_names[2] = strdup("Grace"); */

    printf("\nDynamic string array (TODO: implement):\n");
    if (dynamic_names != NULL) {
        for (int i = 0; i < dynamic_count; i++) {
            if (dynamic_names[i] != NULL) {
                printf("  dynamic_names[%d] = %s\n", i, dynamic_names[i]);
            }
        }

        /* TODO: Free all strings and the array */
        /* for (int i = 0; i < dynamic_count; i++) { free(dynamic_names[i]); } */
        /* free(dynamic_names); */
    }
}

/* ============================================================================
 * CHALLENGE: Linked List with Double Pointer [HARD]
 * ============================================================================
 *
 * This pattern is common in GIOS: using double pointers to modify list heads.
 */

typedef struct Node {
    int data;
    struct Node *next;
} Node;

/*
 * TODO: Implement insert at head using double pointer.
 * This allows modifying the head pointer itself.
 */
void insert_at_head(Node **head, int data) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate new node
     * 2. Set new node's data
     * 3. Set new node's next to current head (*head)
     * 4. Update head to point to new node (*head = new_node)
     */
}

/*
 * TODO: Implement delete node using double pointer.
 * This elegant approach uses a pointer to the link, not to the node.
 */
void delete_node(Node **head, int data) {
    /* TODO: Implement this function
     *
     * Elegant approach using pointer to pointer:
     * Node **current = head;
     * while (*current != NULL) {
     *     if ((*current)->data == data) {
     *         Node *to_delete = *current;
     *         *current = (*current)->next;
     *         free(to_delete);
     *         return;
     *     }
     *     current = &((*current)->next);
     * }
     */
}

void print_list(Node *head) {
    printf("List: ");
    while (head != NULL) {
        printf("%d -> ", head->data);
        head = head->next;
    }
    printf("NULL\n");
}

void free_list(Node *head) {
    while (head != NULL) {
        Node *temp = head;
        head = head->next;
        free(temp);
    }
}

/* ============================================================================
 * MAIN FUNCTION
 * ============================================================================
 */
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 03: Double Pointers\n");
    printf("================================================\n");

    exercise1_double_pointer_basics();
    exercise2_modifying_pointers();
    exercise3_double_pointer_functions();
    exercise4_2d_arrays();
    exercise5_string_array();

    /* Test linked list operations */
    printf("\n=== Challenge: Linked List with Double Pointer ===\n");
    Node *list = NULL;

    insert_at_head(&list, 30);
    insert_at_head(&list, 20);
    insert_at_head(&list, 10);
    print_list(list);  /* Expected: 10 -> 20 -> 30 -> NULL */

    delete_node(&list, 20);
    print_list(list);  /* Expected: 10 -> 30 -> NULL */

    delete_node(&list, 10);
    print_list(list);  /* Expected: 30 -> NULL */

    free_list(list);

    printf("\n================================================\n");
    printf("  Double Pointers Complete!\n");
    printf("  Next: pointer_bugs.c\n");
    printf("================================================\n\n");

    return 0;
}
