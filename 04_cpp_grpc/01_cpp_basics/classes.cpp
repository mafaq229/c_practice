/*
 * CS-6200 Preparation - Module 01: C++ Classes
 *
 * C++ classes for C programmers.
 *
 * Compile: clang++ -std=c++17 -Wall -Wextra classes.cpp -o classes
 * Run:     ./classes
 *
 * Difficulty: [MEDIUM]
 */

#include <iostream>
#include <string>
#include <cstring>

using namespace std;  // In real code, prefer explicit std:: prefixes

/* ============================================================================
 * CONCEPT: Classes vs Structs
 * ============================================================================
 *
 * In C++, struct and class are almost identical.
 * The only difference: struct defaults to public, class defaults to private.
 *
 * Use struct for simple data containers.
 * Use class for objects with behavior (methods).
 */

/* ============================================================================
 * EXERCISE 1: Simple class (like a C struct with functions)
 * ============================================================================
 */

class Point {
public:  // Public members - accessible from outside
    int x;
    int y;

    // Constructor - called when object is created
    Point(int x_val, int y_val) {
        x = x_val;
        y = y_val;
        cout << "Point constructor called: (" << x << ", " << y << ")" << endl;
    }

    // Default constructor
    Point() : x(0), y(0) {
        cout << "Point default constructor called" << endl;
    }

    // Destructor - called when object is destroyed
    ~Point() {
        cout << "Point destructor called: (" << x << ", " << y << ")" << endl;
    }

    // Member function (method)
    void print() {
        cout << "Point(" << x << ", " << y << ")" << endl;
    }

    // Method that modifies the object
    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    // Const method - promises not to modify object
    double distance() const {
        return sqrt(x*x + y*y);
    }
};

void exercise1_basic_class() {
    cout << "\n=== Exercise 1: Basic Class ===" << endl;

    // Stack allocation (like C struct)
    Point p1(3, 4);
    p1.print();
    cout << "Distance from origin: " << p1.distance() << endl;

    // Default constructor
    Point p2;
    p2.print();

    p2.move(5, 5);
    p2.print();

    cout << "End of function - destructors will be called" << endl;
}  // Destructors called here

/* ============================================================================
 * EXERCISE 2: Encapsulation (private members)
 * ============================================================================
 */

class BankAccount {
private:  // Private members - only accessible from within the class
    string owner;
    double balance;

public:
    // Constructor with initialization list (preferred style)
    BankAccount(const string& owner_name, double initial_balance)
        : owner(owner_name), balance(initial_balance) {
        cout << "Created account for " << owner << endl;
    }

    // Getter methods (accessors)
    string getOwner() const { return owner; }
    double getBalance() const { return balance; }

    // Methods that modify state
    bool deposit(double amount) {
        if (amount <= 0) {
            cout << "Invalid deposit amount" << endl;
            return false;
        }
        balance += amount;
        return true;
    }

    bool withdraw(double amount) {
        if (amount <= 0 || amount > balance) {
            cout << "Invalid withdrawal" << endl;
            return false;
        }
        balance -= amount;
        return true;
    }

    void print() const {
        cout << "Account[" << owner << "]: $" << balance << endl;
    }
};

void exercise2_encapsulation() {
    cout << "\n=== Exercise 2: Encapsulation ===" << endl;

    BankAccount account("John Doe", 1000.0);
    account.print();

    account.deposit(500);
    account.print();

    account.withdraw(200);
    account.print();

    // Can't access private members:
    // account.balance = 1000000;  // Error!
}

/* ============================================================================
 * EXERCISE 3: RAII (Resource Acquisition Is Initialization)
 * ============================================================================
 *
 * This is THE most important C++ concept!
 * Resources (memory, files, locks) are tied to object lifetime.
 * Constructor acquires, destructor releases.
 * No more forgetting to free/close/unlock!
 */

class FileWrapper {
private:
    FILE* file;
    string filename;

public:
    // Constructor opens the file
    FileWrapper(const string& fname, const char* mode)
        : file(nullptr), filename(fname) {

        file = fopen(fname.c_str(), mode);
        if (file) {
            cout << "Opened file: " << filename << endl;
        } else {
            cout << "Failed to open: " << filename << endl;
        }
    }

    // Destructor closes the file
    ~FileWrapper() {
        if (file) {
            fclose(file);
            cout << "Closed file: " << filename << endl;
        }
    }

    // Prevent copying (advanced - but important!)
    FileWrapper(const FileWrapper&) = delete;
    FileWrapper& operator=(const FileWrapper&) = delete;

    // Check if file is open
    bool isOpen() const { return file != nullptr; }

    // Write to file
    void write(const string& text) {
        if (file) {
            fprintf(file, "%s", text.c_str());
        }
    }

    // Read line from file
    string readLine() {
        if (!file) return "";

        char buffer[256];
        if (fgets(buffer, sizeof(buffer), file)) {
            return string(buffer);
        }
        return "";
    }
};

void exercise3_raii() {
    cout << "\n=== Exercise 3: RAII ===" << endl;

    {
        FileWrapper writer("/tmp/test_cpp.txt", "w");
        if (writer.isOpen()) {
            writer.write("Hello from C++!\n");
            writer.write("RAII is great!\n");
        }
    }  // Destructor automatically closes file here!

    cout << "File was automatically closed by destructor" << endl;

    {
        FileWrapper reader("/tmp/test_cpp.txt", "r");
        if (reader.isOpen()) {
            cout << "Read: " << reader.readLine();
            cout << "Read: " << reader.readLine();
        }
    }  // File closed again
}

/* ============================================================================
 * EXERCISE 4: Inheritance
 * ============================================================================
 */

class Shape {
protected:  // Accessible by derived classes
    string name;

public:
    Shape(const string& n) : name(n) {}

    virtual ~Shape() {  // Virtual destructor - important for polymorphism!
        cout << "~Shape: " << name << endl;
    }

    // Pure virtual function - makes this an abstract class
    virtual double area() const = 0;

    // Virtual function with default implementation
    virtual void print() const {
        cout << name << " (area: " << area() << ")" << endl;
    }
};

class Rectangle : public Shape {
private:
    double width, height;

public:
    Rectangle(double w, double h)
        : Shape("Rectangle"), width(w), height(h) {}

    ~Rectangle() override {
        cout << "~Rectangle" << endl;
    }

    double area() const override {
        return width * height;
    }
};

class Circle : public Shape {
private:
    double radius;

public:
    Circle(double r)
        : Shape("Circle"), radius(r) {}

    ~Circle() override {
        cout << "~Circle" << endl;
    }

    double area() const override {
        return 3.14159 * radius * radius;
    }
};

void exercise4_inheritance() {
    cout << "\n=== Exercise 4: Inheritance ===" << endl;

    Rectangle rect(4, 5);
    Circle circle(3);

    rect.print();
    circle.print();

    // Polymorphism - treat different types uniformly
    Shape* shapes[] = {&rect, &circle};
    for (auto shape : shapes) {
        shape->print();
    }
}

/* ============================================================================
 * EXERCISE 5: References vs Pointers
 * ============================================================================
 */

void modify_by_pointer(int* ptr) {
    *ptr = 100;
}

void modify_by_reference(int& ref) {
    ref = 200;
}

void exercise5_references() {
    cout << "\n=== Exercise 5: References ===" << endl;

    int value = 10;
    cout << "Original: " << value << endl;

    // Pointer style (C-like)
    modify_by_pointer(&value);
    cout << "After pointer: " << value << endl;

    // Reference style (C++ preferred)
    modify_by_reference(value);
    cout << "After reference: " << value << endl;

    // References are like "auto-dereferencing pointers"
    int& alias = value;  // alias IS value
    alias = 300;
    cout << "After alias: " << value << endl;

    // Key differences:
    // - References cannot be null
    // - References cannot be reassigned
    // - References don't need explicit dereferencing
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main() {
    cout << "\n================================================" << endl;
    cout << "  Module 01: C++ Classes" << endl;
    cout << "================================================" << endl;

    exercise1_basic_class();
    exercise2_encapsulation();
    exercise3_raii();
    exercise4_inheritance();
    exercise5_references();

    cout << "\n================================================" << endl;
    cout << "  Classes Complete!" << endl;
    cout << "  Key takeaways:" << endl;
    cout << "  - Constructors/destructors manage lifecycle" << endl;
    cout << "  - RAII ties resources to object lifetime" << endl;
    cout << "  - virtual enables polymorphism" << endl;
    cout << "  - References are safer than pointers" << endl;
    cout << "================================================\n" << endl;

    return 0;
}
