/*
CS-6200 Preparation - Module 04: Custom Allocator

Understanding how memory allocators work helps you understand
malloc/free and write more efficient code.

This exercise implements a simple memory pool allocator.

Compile: clang -Wall -Wextra -g custom_allocator.c -o custom_allocator
Run:     ./custom_allocator

Difficulty: [HARD] - This is a challenge exercise
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ============================================================================
CONCEPT: Memory Pool Allocator
============================================================================

A memory pool pre-allocates a large block and manages it internally.

Advantages:
  - Faster allocation (no system call per allocation)
  - Better cache locality
  - Can free all at once
  - Avoids fragmentation for fixed-size allocations

This is commonly used in:
  - Game engines
  - Network servers
  - Database systems
  - Operating systems (GIOS covers this!)
*/

/* ============================================================================
EXERCISE 1: Fixed-Size Block Pool [MEDIUM]
============================================================================

Allocator for fixed-size blocks. Simple but useful.
*/

#define BLOCK_SIZE 64
#define POOL_SIZE 1024  /* 1024 blocks */

typedef struct {
    uint8_t memory[BLOCK_SIZE * POOL_SIZE];  /* Raw memory */
    uint8_t used[POOL_SIZE];                  /* 1 if block is used, 0 if free */
    int num_allocated;
} FixedPool;

/*
TODO: Initialize the pool.
Mark all blocks as free.
*/
void fixed_pool_init(FixedPool *pool) {
    // memset(pool->used, 0, sizeof(pool->used)); // fastest, simplest way to zero a byte array
    // can use int below but size_t is the standard unsigned type for sizes and array indices. 
    size_t count = sizeof(pool->used) / (pool->used[0]);
    for (size_t i = 0; i < count; i++) {
        pool->used[i] = 0;
    }
    pool->num_allocated = 0;
}

/*
TODO: Allocate a block from the pool.
Returns pointer to block, or NULL if pool is full.
*/
void *fixed_pool_alloc(FixedPool *pool) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Find a free block (used[i] == 0)
     * 2. Mark it as used
     * 3. Increment num_allocated
     * 4. Return pointer to that block in memory[]
     *
     * HINT: Block i starts at memory[i * BLOCK_SIZE]
     */
    for (int i = 0; i < POOL_SIZE; i++) {
        if (pool->used[i] == 0) {
            pool->used[i] = 1;
            pool->num_allocated++;
            return &pool->memory[i * BLOCK_SIZE];
        }
    }
    return NULL;
}

/*
TODO: Free a block back to the pool.
*/
void fixed_pool_free(FixedPool *pool, void *ptr) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Calculate which block this pointer belongs to
     * 2. Mark it as free
     * 3. Decrement num_allocated
     *
     * HINT: Block index = (ptr - pool->memory) / BLOCK_SIZE
     */
    int index = (int)(((uint8_t *)ptr - pool->memory) / BLOCK_SIZE);
    pool->used[index] = 0;
    pool->num_allocated--;
}

/*
TODO: Get pool statistics.
*/
int fixed_pool_num_free(FixedPool *pool) {
    return POOL_SIZE - pool->num_allocated;
}

void exercise1_fixed_pool(void) {
    printf("\n=== Exercise 1: Fixed-Size Block Pool ===\n");

    FixedPool pool;
    fixed_pool_init(&pool);

    printf("Pool initialized: %d blocks available\n", fixed_pool_num_free(&pool));

    /* Allocate some blocks */
    void *blocks[10];
    for (int i = 0; i < 10; i++) {
        blocks[i] = fixed_pool_alloc(&pool);
        if (blocks[i] == NULL) {
            printf("TODO: Implement fixed_pool_alloc\n");
            return;
        }
        /* Write some data */
        memset(blocks[i], i, BLOCK_SIZE);
    }

    printf("After allocating 10 blocks: %d free\n", fixed_pool_num_free(&pool));

    /* Free some blocks */
    fixed_pool_free(&pool, blocks[3]);
    fixed_pool_free(&pool, blocks[7]);

    printf("After freeing 2 blocks: %d free\n", fixed_pool_num_free(&pool));

    /* Allocate again - should reuse freed blocks */
    void *reused = fixed_pool_alloc(&pool);
    printf("Reused block allocated: %s\n", reused != NULL ? "yes" : "no");

    printf("Expected: 1024 -> 1014 -> 1016 -> reused\n");
}

/* ============================================================================
EXERCISE 2: Arena Allocator (Bump Allocator) [MEDIUM]
============================================================================

Simplest possible allocator: just bump a pointer forward.
Can only free all at once.
*/

typedef struct {
    uint8_t *memory;     /* Base of allocated memory */
    size_t capacity;     /* Total capacity */
    size_t offset;       /* Current offset (next free position) */
} Arena;

/*
TODO: Create an arena with given capacity.
*/
Arena *arena_create(size_t capacity) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate Arena struct
     * 2. Allocate memory buffer of given capacity
     * 3. Set offset to 0
     * 4. Return arena (or NULL on failure)
     */
    Arena *arena = malloc(sizeof(Arena)); // struct Arena *a; without typedef
    if (!arena) return NULL;

    arena->memory = malloc(capacity);
    if (!arena->memory) {
        free(arena);
        return NULL;
    }

    arena->capacity = capacity;
    arena->offset = 0;
    return arena;  /* TODO: Fix this */
}

/*
TODO: Allocate memory from the arena.
Returns NULL if not enough space.
*/
void *arena_alloc(Arena *arena, size_t size) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Check if there's enough space: offset + size <= capacity
     * 2. Save current offset as the return pointer
     * 3. Bump offset forward by size
     * 4. Return the saved pointer
     *
     * HINT: void *ptr = arena->memory + arena->offset;
     */
    if (arena->offset + size > arena->capacity) return NULL;
    void *ptr = arena->memory + arena->offset; // ptr is pointed to memory address at offset
    arena->offset += size;
    return ptr;  /* TODO: Fix this */
}

/*
TODO: Reset the arena (free all allocations at once).
*/
void arena_reset(Arena *arena) {
    /* TODO: Just set offset back to 0 */
    arena->offset = 0;
}

/*
TODO: Destroy the arena.
*/
void arena_destroy(Arena *arena) {
    /* TODO: Free memory buffer and arena struct */
    free(arena->memory);
    free(arena);
}

void exercise2_arena(void) {
    printf("\n=== Exercise 2: Arena Allocator ===\n");

    Arena *arena = arena_create(4096);
    if (arena == NULL) {
        printf("TODO: Implement arena_create\n");
        return;
    }

    /* Allocate various sizes */
    int *numbers = arena_alloc(arena, 10 * sizeof(int));
    char *string = arena_alloc(arena, 100);
    double *values = arena_alloc(arena, 5 * sizeof(double));

    if (numbers == NULL || string == NULL || values == NULL) {
        printf("TODO: Implement arena_alloc\n");
        arena_destroy(arena);
        return;
    }

    /* Use the memory */
    for (int i = 0; i < 10; i++) numbers[i] = i * i;
    strcpy(string, "Hello from arena!");
    for (int i = 0; i < 5; i++) values[i] = i * 1.5;

    printf("numbers[5] = %d (expected: 25)\n", numbers[5]);
    printf("string = %s\n", string);
    printf("values[3] = %.1f (expected: 4.5)\n", values[3]);

    /* Reset and reuse */
    arena_reset(arena);
    printf("Arena reset - all allocations freed at once!\n");

    /* Can allocate again */
    int *new_numbers = arena_alloc(arena, 100 * sizeof(int));
    printf("New allocation after reset: %s\n",
           new_numbers != NULL ? "success" : "failed");

    arena_destroy(arena);
}

/* ============================================================================
EXERCISE 3: Free List Allocator [HARD]
============================================================================

A more sophisticated allocator that maintains a free list.
This is closer to how malloc actually works.
*/

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock *next;
} FreeBlock;

typedef struct {
    uint8_t *memory;
    size_t capacity;
    FreeBlock *free_list;
} FreeListAllocator;

/*
TODO: Initialize the free list allocator.
*/
FreeListAllocator *freelist_create(size_t capacity) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate the allocator struct
     * 2. Allocate the memory buffer
     * 3. Initialize free_list to point to the entire buffer as one big free block
     *
     * HINT: Cast memory to FreeBlock* to set up the initial free block
     */
    FreeListAllocator *allocator = malloc(sizeof(FreeListAllocator));
    if (!allocator) return NULL;
    allocator->memory = malloc(capacity);
    if (!allocator->memory) {
        free(allocator);
        return NULL;
    }
    allocator->capacity = capacity;
    // Treat the start of the buffer as a FreeBlock header for the free list
    FreeBlock *free_block = (FreeBlock *)allocator->memory;
    free_block->size = capacity;
    free_block->next = NULL;  
    allocator->free_list = free_block;
    return allocator;  /* TODO: Fix this */
}

/*
TODO: Allocate memory using first-fit strategy.
*/
void *freelist_alloc(FreeListAllocator *alloc, size_t size) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Walk the free list to find a block big enough
     * 2. If found, remove it from the free list (or split if much larger)
     * 3. Store the size at the beginning of the block (for freeing later)
     * 4. Return pointer after the size field
     *
     * This is complex! Start simple without splitting.
     */
    // allocator needs space not just for the user’s data, but also to store a header (the block size).
    size_t total_size = size + sizeof(size_t);
    FreeBlock *prev = NULL;
    FreeBlock *curr = alloc->free_list;

    while (curr != NULL) {
        if (curr->size >= total_size) {
            size_t remaining_size = curr->size - total_size;
            if (remaining_size >= sizeof(FreeBlock)) {
                FreeBlock *remaining = (FreeBlock *)((uint8_t *)curr + total_size);
                remaining->size = remaining_size;
                remaining->next = curr->next;
                if (prev != NULL) {
                    prev->next = remaining;
                } else {
                    alloc->free_list = remaining;
                }
                *((size_t *)curr) = total_size;
            } else {
                if (prev != NULL) {
                    prev->next = curr->next;
                } else {
                    alloc->free_list = curr->next;
                }
                *((size_t *)curr) = curr->size;
            }
            return (uint8_t *)curr + sizeof(size_t);
        }
        prev = curr;
        curr = curr->next;
    }

    return NULL;  /* TODO: Fix this */
}

/*
TODO: Free memory back to the allocator.
*/
void freelist_free(FreeListAllocator *alloc, void *ptr) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Get the block header (ptr - sizeof(size_t))
     * 2. Add this block to the free list
     *
     * Advanced: Coalesce with adjacent free blocks
     */
    if (ptr == NULL) return;

    uint8_t *block_start = (uint8_t *)ptr - sizeof(size_t);
    size_t block_size = *((size_t *)block_start);
    FreeBlock *block = (FreeBlock *)block_start;

    block->size = block_size;
    block->next = alloc->free_list;
    alloc->free_list = block;
}

void freelist_destroy(FreeListAllocator *alloc) {
    if (alloc == NULL) return;
    free(alloc->memory);
    free(alloc);
}

void exercise3_freelist(void) {
    printf("\n=== Exercise 3: Free List Allocator ===\n");

    FreeListAllocator *alloc = freelist_create(4096);
    if (alloc == NULL) {
        printf("TODO: Implement freelist_create\n");
        return;
    }

    void *a = freelist_alloc(alloc, 100);
    void *b = freelist_alloc(alloc, 200);
    void *c = freelist_alloc(alloc, 50);

    if (a == NULL || b == NULL || c == NULL) {
        printf("TODO: Implement freelist_alloc\n");
        freelist_destroy(alloc);
        return;
    }

    printf("Allocated: a=%p, b=%p, c=%p\n", a, b, c);

    freelist_free(alloc, b);
    printf("Freed b\n");

    void *d = freelist_alloc(alloc, 150);
    printf("Allocated d=%p (should reuse b's space if properly implemented)\n", d);

    freelist_destroy(alloc);
}

/* ============================================================================
SUMMARY: Allocator Comparison
============================================================================

| Allocator     | Alloc Time | Free Time | Individual Free | Fragmentation |
|---------------|------------|-----------|-----------------|---------------|
| Fixed Pool    | O(n)       | O(1)      | Yes             | None (fixed)  |
| Arena/Bump    | O(1)       | O(1)*     | No (all at once)| None          |
| Free List     | O(n)       | O(1)      | Yes             | Yes           |
| malloc        | Varies     | Varies    | Yes             | Managed       |

* Arena free is O(1) but frees everything at once

When to use each:
- Fixed Pool: Many allocations of same size (e.g., network packets)
- Arena: Temporary allocations with known lifetime (e.g., request handling)
- Free List: General purpose, like malloc
*/

/* ============================================================================
MAIN FUNCTION
============================================================================
*/
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Module 04: Custom Allocator\n");
    printf("================================================\n");
    printf("\nThis is a challenge module. Take your time!\n");

    exercise1_fixed_pool();
    exercise2_arena();
    exercise3_freelist();

    printf("\n================================================\n");
    printf("  Custom Allocator Complete!\n");
    printf("  This knowledge helps understand malloc/free.\n");
    printf("  Next: Module 05 - Strings\n");
    printf("================================================\n\n");

    return 0;
}
