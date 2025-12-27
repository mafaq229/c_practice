/*
 * CS-6200 Preparation - Module 01: STL Containers
 *
 * Standard Template Library containers - no more manual memory management!
 *
 * Compile: clang++ -std=c++17 -Wall -Wextra stl_containers.cpp -o stl_containers
 * Run:     ./stl_containers
 *
 * Difficulty: [MEDIUM]
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <algorithm>

using namespace std;

/* ============================================================================
 * EXERCISE 1: std::string (vs char*)
 * ============================================================================
 */

void exercise1_strings() {
    cout << "\n=== Exercise 1: std::string ===" << endl;

    // Creating strings
    string s1 = "Hello";
    string s2("World");
    string s3 = s1 + ", " + s2 + "!";  // Concatenation is easy!

    cout << "s3: " << s3 << endl;
    cout << "Length: " << s3.length() << endl;
    cout << "Character at 0: " << s3[0] << endl;

    // Substring
    string sub = s3.substr(0, 5);
    cout << "Substring: " << sub << endl;

    // Find
    size_t pos = s3.find("World");
    if (pos != string::npos) {
        cout << "Found 'World' at position: " << pos << endl;
    }

    // Convert to C string (for C functions)
    const char* c_str = s3.c_str();
    cout << "C string: " << c_str << endl;

    // Compare
    if (s1 == "Hello") {
        cout << "Strings can use == for comparison!" << endl;
    }

    // No need to free - automatic memory management!
}

/* ============================================================================
 * EXERCISE 2: std::vector (dynamic array)
 * ============================================================================
 */

void exercise2_vectors() {
    cout << "\n=== Exercise 2: std::vector ===" << endl;

    // Creating vectors
    vector<int> numbers;  // Empty vector

    // Adding elements
    numbers.push_back(10);
    numbers.push_back(20);
    numbers.push_back(30);

    cout << "Size: " << numbers.size() << endl;

    // Initialize with values
    vector<int> primes = {2, 3, 5, 7, 11, 13};

    // Access elements
    cout << "First: " << primes[0] << endl;
    cout << "Last: " << primes.back() << endl;

    // Safe access with bounds checking
    try {
        cout << "primes.at(10): " << primes.at(10) << endl;
    } catch (const out_of_range& e) {
        cout << "Exception: " << e.what() << endl;
    }

    // Iterating - modern C++ style
    cout << "Primes: ";
    for (int p : primes) {  // Range-based for loop
        cout << p << " ";
    }
    cout << endl;

    // Iterating with index
    for (size_t i = 0; i < primes.size(); i++) {
        cout << "primes[" << i << "] = " << primes[i] << endl;
    }

    // Vector of strings
    vector<string> names = {"Alice", "Bob", "Charlie"};
    names.push_back("Diana");

    cout << "Names: ";
    for (const auto& name : names) {  // auto deduces type, const& avoids copy
        cout << name << " ";
    }
    cout << endl;
}

/* ============================================================================
 * EXERCISE 3: std::map (sorted dictionary)
 * ============================================================================
 */

void exercise3_maps() {
    cout << "\n=== Exercise 3: std::map ===" << endl;

    // Map from string to int
    map<string, int> ages;

    // Insert values
    ages["Alice"] = 25;
    ages["Bob"] = 30;
    ages.insert({"Charlie", 35});

    // Access
    cout << "Alice's age: " << ages["Alice"] << endl;

    // Check if key exists
    if (ages.find("Diana") == ages.end()) {
        cout << "Diana not found" << endl;
    }

    // Count (returns 0 or 1 for map)
    if (ages.count("Bob") > 0) {
        cout << "Bob exists" << endl;
    }

    // Iterate
    cout << "All ages:" << endl;
    for (const auto& [name, age] : ages) {  // C++17 structured binding
        cout << "  " << name << ": " << age << endl;
    }

    // Map is sorted by key
    ages["Zoe"] = 20;
    ages["Adam"] = 40;

    cout << "Sorted order:" << endl;
    for (const auto& pair : ages) {
        cout << "  " << pair.first << ": " << pair.second << endl;
    }
}

/* ============================================================================
 * EXERCISE 4: std::unordered_map (hash table)
 * ============================================================================
 */

void exercise4_unordered_maps() {
    cout << "\n=== Exercise 4: std::unordered_map ===" << endl;

    // Faster than map for lookups (O(1) average vs O(log n))
    unordered_map<string, string> capitals;

    capitals["France"] = "Paris";
    capitals["Japan"] = "Tokyo";
    capitals["USA"] = "Washington D.C.";
    capitals["Germany"] = "Berlin";

    cout << "Capital of Japan: " << capitals["Japan"] << endl;

    // Iterate (order is not guaranteed!)
    for (const auto& [country, capital] : capitals) {
        cout << country << " -> " << capital << endl;
    }

    // Use unordered_map when:
    // - You need O(1) lookups
    // - You don't need sorted order
}

/* ============================================================================
 * EXERCISE 5: std::set (sorted unique elements)
 * ============================================================================
 */

void exercise5_sets() {
    cout << "\n=== Exercise 5: std::set ===" << endl;

    set<int> numbers;

    numbers.insert(5);
    numbers.insert(2);
    numbers.insert(8);
    numbers.insert(2);  // Duplicate - ignored!

    cout << "Size: " << numbers.size() << endl;  // 3, not 4

    // Sets are sorted
    cout << "Numbers: ";
    for (int n : numbers) {
        cout << n << " ";
    }
    cout << endl;

    // Check membership
    if (numbers.count(5) > 0) {
        cout << "5 is in the set" << endl;
    }

    // C++20: contains()
    // if (numbers.contains(5)) { ... }

    // Erase
    numbers.erase(2);
    cout << "After erasing 2: ";
    for (int n : numbers) {
        cout << n << " ";
    }
    cout << endl;
}

/* ============================================================================
 * EXERCISE 6: std::queue (FIFO)
 * ============================================================================
 */

void exercise6_queues() {
    cout << "\n=== Exercise 6: std::queue ===" << endl;

    queue<string> tasks;

    tasks.push("Task 1");
    tasks.push("Task 2");
    tasks.push("Task 3");

    cout << "Processing tasks:" << endl;
    while (!tasks.empty()) {
        cout << "  Processing: " << tasks.front() << endl;
        tasks.pop();
    }

    // Priority queue
    cout << "\nPriority queue:" << endl;
    priority_queue<int> pq;  // Max heap by default

    pq.push(3);
    pq.push(1);
    pq.push(4);
    pq.push(1);
    pq.push(5);

    cout << "Popping (highest first): ";
    while (!pq.empty()) {
        cout << pq.top() << " ";
        pq.pop();
    }
    cout << endl;
}

/* ============================================================================
 * EXERCISE 7: Algorithms
 * ============================================================================
 */

void exercise7_algorithms() {
    cout << "\n=== Exercise 7: STL Algorithms ===" << endl;

    vector<int> nums = {5, 2, 8, 1, 9, 3, 7, 4, 6};

    // Sort
    sort(nums.begin(), nums.end());
    cout << "Sorted: ";
    for (int n : nums) cout << n << " ";
    cout << endl;

    // Reverse
    reverse(nums.begin(), nums.end());
    cout << "Reversed: ";
    for (int n : nums) cout << n << " ";
    cout << endl;

    // Find
    auto it = find(nums.begin(), nums.end(), 5);
    if (it != nums.end()) {
        cout << "Found 5 at index: " << (it - nums.begin()) << endl;
    }

    // Min/Max
    cout << "Min: " << *min_element(nums.begin(), nums.end()) << endl;
    cout << "Max: " << *max_element(nums.begin(), nums.end()) << endl;

    // Count
    vector<int> data = {1, 2, 2, 3, 2, 4, 2};
    cout << "Count of 2: " << count(data.begin(), data.end(), 2) << endl;

    // Remove (doesn't resize - use erase-remove idiom)
    data.erase(remove(data.begin(), data.end(), 2), data.end());
    cout << "After removing 2s: ";
    for (int n : data) cout << n << " ";
    cout << endl;
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main() {
    cout << "\n================================================" << endl;
    cout << "  Module 01: STL Containers" << endl;
    cout << "================================================" << endl;

    exercise1_strings();
    exercise2_vectors();
    exercise3_maps();
    exercise4_unordered_maps();
    exercise5_sets();
    exercise6_queues();
    exercise7_algorithms();

    cout << "\n================================================" << endl;
    cout << "  STL Containers Complete!" << endl;
    cout << "  No more manual memory management!" << endl;
    cout << "================================================\n" << endl;

    return 0;
}
