/*
CS-6200 Preparation - Module 06: Linked List

Linked lists are fundamental data structures used throughout GIOS.
Thread pools, request queues, and many other components use them.

Compile: clang -Wall -Wextra -g linked_list.c -o linked_list
Run:     ./linked_list

Difficulty: [MEDIUM]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
SINGLY LINKED LIST
============================================================================
*/

typedef struct SNode {
    int data;
    struct SNode *next;
} SNode;

typedef struct {
    SNode *head;
    SNode *tail;
    int size;
} SinglyLinkedList;

/*
TODO: Initialize the list.
*/
void slist_init(SinglyLinkedList *list) {
    /* TODO: Set head and tail to NULL, size to 0 */
}

/*
TODO: Append to end of list.
*/
int slist_append(SinglyLinkedList *list, int data) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate new node
     * 2. Set data, next = NULL
     * 3. If list is empty, set head and tail to new node
     * 4. Otherwise, update tail->next and tail
     * 5. Increment size
     * 6. Return 0 on success, -1 on failure
     */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Prepend to beginning of list.
*/
int slist_prepend(SinglyLinkedList *list, int data) {
    /* TODO: Implement this function */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Remove and return first element.
Returns -1 if list is empty (use a different error mechanism in real code).
*/
int slist_pop_front(SinglyLinkedList *list) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Check if list is empty
     * 2. Save head's data
     * 3. Update head to head->next
     * 4. If list is now empty, update tail
     * 5. Free old head
     * 6. Decrement size
     * 7. Return data
     */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Find element by value.
Returns pointer to node or NULL if not found.
*/
SNode *slist_find(SinglyLinkedList *list, int data) {
    /* TODO: Implement this function */

    return NULL;  /* TODO: Fix this */
}

/*
TODO: Remove element by value.
Returns 0 if removed, -1 if not found.
*/
int slist_remove(SinglyLinkedList *list, int data) {
    /* TODO: Implement this function
     *
     * Key insight: Track previous node to update links.
     * Handle special case when removing head.
     */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Free all nodes.
*/
void slist_free(SinglyLinkedList *list) {
    /* TODO: Implement this function */
}

void slist_print(SinglyLinkedList *list) {
    printf("[");
    SNode *curr = list->head;
    while (curr != NULL) {
        printf("%d", curr->data);
        if (curr->next != NULL) printf(" -> ");
        curr = curr->next;
    }
    printf("] (size=%d)\n", list->size);
}

/* ============================================================================
DOUBLY LINKED LIST
============================================================================
*/

typedef struct DNode {
    int data;
    struct DNode *prev;
    struct DNode *next;
} DNode;

typedef struct {
    DNode *head;
    DNode *tail;
    int size;
} DoublyLinkedList;

/*
TODO: Initialize doubly linked list.
*/
void dlist_init(DoublyLinkedList *list) {
    /* TODO: Implement this function */
}

/*
TODO: Append to doubly linked list.
*/
int dlist_append(DoublyLinkedList *list, int data) {
    /* TODO: Implement this function
     *
     * Remember to set both next and prev pointers!
     */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Remove from end of doubly linked list.
*/
int dlist_pop_back(DoublyLinkedList *list) {
    /* TODO: Implement this function
     *
     * Advantage over singly linked: O(1) removal from end!
     */

    return -1;  /* TODO: Fix this */
}

/*
TODO: Remove a specific node.
This is O(1) because we have prev pointer!
*/
void dlist_remove_node(DoublyLinkedList *list, DNode *node) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Update prev's next (or head if node is first)
     * 2. Update next's prev (or tail if node is last)
     * 3. Free node
     * 4. Decrement size
     */
}

void dlist_free(DoublyLinkedList *list) {
    DNode *curr = list->head;
    while (curr != NULL) {
        DNode *next = curr->next;
        free(curr);
        curr = next;
    }
    list->head = list->tail = NULL;
    list->size = 0;
}

void dlist_print(DoublyLinkedList *list) {
    printf("[");
    DNode *curr = list->head;
    while (curr != NULL) {
        printf("%d", curr->data);
        if (curr->next != NULL) printf(" <-> ");
        curr = curr->next;
    }
    printf("] (size=%d)\n", list->size);
}

/* ============================================================================
TEST FUNCTIONS
============================================================================
*/

void test_singly_linked(void) {
    printf("\n=== Singly Linked List ===\n");

    SinglyLinkedList list;
    slist_init(&list);

    slist_append(&list, 10);
    slist_append(&list, 20);
    slist_append(&list, 30);
    printf("After appending 10, 20, 30: ");
    slist_print(&list);

    slist_prepend(&list, 5);
    printf("After prepending 5: ");
    slist_print(&list);

    int val = slist_pop_front(&list);
    printf("Popped front: %d, list: ", val);
    slist_print(&list);

    slist_remove(&list, 20);
    printf("After removing 20: ");
    slist_print(&list);

    SNode *found = slist_find(&list, 30);
    printf("Find 30: %s\n", found ? "found" : "not found");

    slist_free(&list);
    printf("List freed.\n");
}

void test_doubly_linked(void) {
    printf("\n=== Doubly Linked List ===\n");

    DoublyLinkedList list;
    dlist_init(&list);

    dlist_append(&list, 10);
    dlist_append(&list, 20);
    dlist_append(&list, 30);
    printf("After appending 10, 20, 30: ");
    dlist_print(&list);

    int val = dlist_pop_back(&list);
    printf("Popped back: %d, list: ", val);
    dlist_print(&list);

    dlist_free(&list);
    printf("List freed.\n");
}

/* ============================================================================
MAIN FUNCTION
============================================================================
*/
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 06: Linked List\n");
    printf("================================================\n");

    test_singly_linked();
    test_doubly_linked();

    printf("\n================================================\n");
    printf("  Linked List Complete!\n");
    printf("  Next: hash_table.c\n");
    printf("================================================\n\n");

    return 0;
}
