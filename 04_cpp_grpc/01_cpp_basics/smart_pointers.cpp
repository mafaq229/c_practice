/*
 * CS-6200 Preparation - Module 01: Smart Pointers
 *
 * Modern C++ memory management - no more memory leaks!
 *
 * Compile: clang++ -std=c++17 -Wall -Wextra smart_pointers.cpp -o smart_pointers
 * Run:     ./smart_pointers
 *
 * Difficulty: [MEDIUM]
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

/* ============================================================================
 * CONCEPT: Smart Pointers
 * ============================================================================
 *
 * Smart pointers are RAII wrappers around raw pointers.
 * They automatically delete the pointed-to object when no longer needed.
 *
 * Types:
 * - unique_ptr: Exclusive ownership (only one pointer to object)
 * - shared_ptr: Shared ownership (multiple pointers, reference counted)
 * - weak_ptr: Non-owning reference to shared_ptr (avoids cycles)
 */

// Simple class for demonstration
class Resource {
private:
    string name;
public:
    Resource(const string& n) : name(n) {
        cout << "Resource '" << name << "' created" << endl;
    }
    ~Resource() {
        cout << "Resource '" << name << "' destroyed" << endl;
    }
    void use() {
        cout << "Using resource '" << name << "'" << endl;
    }
    string getName() const { return name; }
};

/* ============================================================================
 * EXERCISE 1: The Problem with Raw Pointers
 * ============================================================================
 */

void exercise1_raw_pointer_problem() {
    cout << "\n=== Exercise 1: Raw Pointer Problems ===" << endl;

    // Problem 1: Forgetting to delete
    {
        Resource* r = new Resource("Leak");
        r->use();
        // Oops, forgot delete r; - MEMORY LEAK!
        delete r;  // Fixed, but easy to forget
    }

    // Problem 2: Exception safety
    // If an exception is thrown before delete, we leak
    cout << "Raw pointers require careful manual management!" << endl;
}

/* ============================================================================
 * EXERCISE 2: unique_ptr (Exclusive Ownership)
 * ============================================================================
 */

void exercise2_unique_ptr() {
    cout << "\n=== Exercise 2: unique_ptr ===" << endl;

    // Create unique_ptr
    unique_ptr<Resource> r1 = make_unique<Resource>("UniqueResource");
    r1->use();

    // Automatic cleanup when scope ends
    {
        unique_ptr<Resource> r2 = make_unique<Resource>("ScopedResource");
        r2->use();
        // r2 automatically deleted here
    }
    cout << "ScopedResource was automatically deleted" << endl;

    // Cannot copy unique_ptr (compilation error)
    // unique_ptr<Resource> r3 = r1;  // Error!

    // Can move unique_ptr (transfers ownership)
    unique_ptr<Resource> r4 = move(r1);
    // r1 is now nullptr
    if (!r1) {
        cout << "r1 is now nullptr after move" << endl;
    }
    r4->use();

    // Get raw pointer (be careful!)
    Resource* raw = r4.get();
    raw->use();
    // Don't delete raw! unique_ptr owns it

    // Release ownership (rare - you now own the pointer)
    Resource* released = r4.release();
    if (!r4) {
        cout << "r4 is now nullptr after release" << endl;
    }
    delete released;  // We must delete it ourselves now

    cout << "End of function - remaining unique_ptrs cleaned up" << endl;
}

/* ============================================================================
 * EXERCISE 3: shared_ptr (Shared Ownership)
 * ============================================================================
 */

void exercise3_shared_ptr() {
    cout << "\n=== Exercise 3: shared_ptr ===" << endl;

    shared_ptr<Resource> s1 = make_shared<Resource>("SharedResource");
    cout << "Reference count: " << s1.use_count() << endl;

    {
        shared_ptr<Resource> s2 = s1;  // Copy - increases ref count
        cout << "After copy, ref count: " << s1.use_count() << endl;

        shared_ptr<Resource> s3 = s1;  // Another copy
        cout << "After another copy, ref count: " << s1.use_count() << endl;

        s2->use();
        // s2 and s3 go out of scope, ref count decreases
    }

    cout << "After scope exit, ref count: " << s1.use_count() << endl;

    // Object deleted when last shared_ptr is destroyed
    cout << "s1 will be destroyed at function end" << endl;
}

/* ============================================================================
 * EXERCISE 4: shared_ptr in containers
 * ============================================================================
 */

void exercise4_shared_in_containers() {
    cout << "\n=== Exercise 4: shared_ptr in Containers ===" << endl;

    vector<shared_ptr<Resource>> resources;

    resources.push_back(make_shared<Resource>("A"));
    resources.push_back(make_shared<Resource>("B"));
    resources.push_back(make_shared<Resource>("C"));

    // Make a copy of one
    shared_ptr<Resource> favorite = resources[1];
    cout << "Favorite ref count: " << favorite.use_count() << endl;

    // Clear the vector
    cout << "Clearing vector..." << endl;
    resources.clear();

    // Favorite still valid!
    cout << "Favorite still valid: ";
    favorite->use();
    cout << "Favorite ref count: " << favorite.use_count() << endl;
}

/* ============================================================================
 * EXERCISE 5: weak_ptr (Breaking Cycles)
 * ============================================================================
 */

class Node;  // Forward declaration

class Node {
public:
    string name;
    // If both were shared_ptr, we'd have a cycle and memory leak!
    shared_ptr<Node> next;   // Owns next node
    weak_ptr<Node> prev;     // Does NOT own previous node

    Node(const string& n) : name(n) {
        cout << "Node '" << name << "' created" << endl;
    }
    ~Node() {
        cout << "Node '" << name << "' destroyed" << endl;
    }
};

void exercise5_weak_ptr() {
    cout << "\n=== Exercise 5: weak_ptr ===" << endl;

    auto node1 = make_shared<Node>("Node1");
    auto node2 = make_shared<Node>("Node2");

    // Create forward links (shared_ptr)
    node1->next = node2;

    // Create backward link (weak_ptr - doesn't increase ref count)
    node2->prev = node1;

    cout << "node1 ref count: " << node1.use_count() << endl;
    cout << "node2 ref count: " << node2.use_count() << endl;

    // Using weak_ptr
    if (auto prev = node2->prev.lock()) {  // lock() returns shared_ptr
        cout << "Node2's prev: " << prev->name << endl;
    } else {
        cout << "Previous node was deleted" << endl;
    }

    // Check if weak_ptr is valid
    cout << "weak_ptr expired? " << (node2->prev.expired() ? "yes" : "no") << endl;
}

/* ============================================================================
 * EXERCISE 6: Factory Pattern with unique_ptr
 * ============================================================================
 */

class Animal {
public:
    virtual ~Animal() = default;
    virtual void speak() = 0;
};

class Dog : public Animal {
public:
    void speak() override { cout << "Woof!" << endl; }
};

class Cat : public Animal {
public:
    void speak() override { cout << "Meow!" << endl; }
};

unique_ptr<Animal> createAnimal(const string& type) {
    if (type == "dog") {
        return make_unique<Dog>();
    } else if (type == "cat") {
        return make_unique<Cat>();
    }
    return nullptr;
}

void exercise6_factory() {
    cout << "\n=== Exercise 6: Factory with unique_ptr ===" << endl;

    auto animal1 = createAnimal("dog");
    auto animal2 = createAnimal("cat");

    if (animal1) animal1->speak();
    if (animal2) animal2->speak();

    // Automatic cleanup
}

/* ============================================================================
 * EXERCISE 7: When to Use Each
 * ============================================================================
 */

void exercise7_guidelines() {
    cout << "\n=== Exercise 7: Usage Guidelines ===" << endl;

    cout << R"(
GUIDELINES:

1. Default to unique_ptr
   - Clear ownership
   - Zero overhead
   - Use for: factory returns, container members

2. Use shared_ptr when truly shared
   - Multiple owners needed
   - Has overhead (ref counting)
   - Use for: shared resources, caches

3. Use weak_ptr to break cycles
   - Observer pattern
   - Cache that might expire
   - Parent-child relationships

4. Avoid raw new/delete
   - Use make_unique / make_shared
   - Exception-safe
   - More efficient

5. Pass by reference, not smart pointer
   - void process(const Resource& r)  // Good
   - void process(shared_ptr<Resource> r)  // Usually wrong
   - Only pass smart pointer if transferring/sharing ownership
)" << endl;
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main() {
    cout << "\n================================================" << endl;
    cout << "  Module 01: Smart Pointers" << endl;
    cout << "================================================" << endl;

    exercise1_raw_pointer_problem();
    exercise2_unique_ptr();
    exercise3_shared_ptr();
    exercise4_shared_in_containers();
    exercise5_weak_ptr();
    exercise6_factory();
    exercise7_guidelines();

    cout << "\n================================================" << endl;
    cout << "  Smart Pointers Complete!" << endl;
    cout << "  No more memory leaks!" << endl;
    cout << "================================================\n" << endl;

    return 0;
}
