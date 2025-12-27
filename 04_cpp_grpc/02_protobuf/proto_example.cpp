/*
 * CS-6200 Preparation - Module 02: Protocol Buffers Example
 *
 * Demonstrates using generated protobuf code.
 *
 * Build (using CMake from parent directory):
 *   mkdir build && cd build && cmake .. && make
 *
 * Difficulty: [MEDIUM]
 */

#include <iostream>
#include <fstream>
#include <string>
#include "messages.pb.h"

using namespace std;
using namespace gios_prep;

/* ============================================================================
 * EXERCISE 1: Creating and using simple messages
 * ============================================================================
 */

void exercise1_simple_message() {
    cout << "\n=== Exercise 1: Simple Message ===" << endl;

    // Create a Person message
    Person person;
    person.set_name("Alice");
    person.set_age(25);
    person.set_email("alice@example.com");

    // Access fields
    cout << "Name: " << person.name() << endl;
    cout << "Age: " << person.age() << endl;
    cout << "Email: " << person.email() << endl;

    // Check if field is set (all have default values in proto3)
    cout << "Has email: " << (person.email().empty() ? "no" : "yes") << endl;

    // Clear a field
    person.clear_email();
    cout << "After clear, email: '" << person.email() << "'" << endl;
}

/* ============================================================================
 * EXERCISE 2: Serialization and deserialization
 * ============================================================================
 */

void exercise2_serialization() {
    cout << "\n=== Exercise 2: Serialization ===" << endl;

    // Create message
    Person original;
    original.set_name("Bob");
    original.set_age(30);
    original.set_email("bob@example.com");

    // Serialize to string
    string serialized;
    if (!original.SerializeToString(&serialized)) {
        cerr << "Failed to serialize" << endl;
        return;
    }

    cout << "Serialized size: " << serialized.size() << " bytes" << endl;
    cout << "Compare to JSON: name + age + email would be ~50 bytes" << endl;

    // Deserialize
    Person parsed;
    if (!parsed.ParseFromString(serialized)) {
        cerr << "Failed to parse" << endl;
        return;
    }

    cout << "Parsed: " << parsed.name() << ", " << parsed.age() << endl;

    // Serialize to file
    const char* filename = "/tmp/person.pb";
    {
        ofstream out(filename, ios::binary);
        if (!original.SerializeToOstream(&out)) {
            cerr << "Failed to write to file" << endl;
        }
    }

    // Deserialize from file
    Person from_file;
    {
        ifstream in(filename, ios::binary);
        if (!from_file.ParseFromIstream(&in)) {
            cerr << "Failed to read from file" << endl;
        }
    }

    cout << "From file: " << from_file.name() << endl;
}

/* ============================================================================
 * EXERCISE 3: Repeated fields
 * ============================================================================
 */

void exercise3_repeated() {
    cout << "\n=== Exercise 3: Repeated Fields ===" << endl;

    AddressBook book;

    // Add people
    Person* p1 = book.add_people();
    p1->set_name("Alice");
    p1->set_age(25);

    Person* p2 = book.add_people();
    p2->set_name("Bob");
    p2->set_age(30);

    // Alternative: create and add
    Person charlie;
    charlie.set_name("Charlie");
    charlie.set_age(35);
    *book.add_people() = charlie;

    // Access
    cout << "Address book has " << book.people_size() << " people:" << endl;
    for (int i = 0; i < book.people_size(); i++) {
        cout << "  " << book.people(i).name() << ": " << book.people(i).age() << endl;
    }

    // Modern iteration
    cout << "\nUsing range-based for:" << endl;
    for (const auto& person : book.people()) {
        cout << "  " << person.name() << endl;
    }
}

/* ============================================================================
 * EXERCISE 4: Nested messages
 * ============================================================================
 */

void exercise4_nested() {
    cout << "\n=== Exercise 4: Nested Messages ===" << endl;

    Course course;
    course.set_code("CS-6200");
    course.set_name("Introduction to Operating Systems");

    // Add assignments
    auto* a1 = course.add_assignments();
    a1->set_name("Project 1: File Transfer");
    a1->set_points(100);
    a1->set_completed(true);

    auto* a2 = course.add_assignments();
    a2->set_name("Project 2: Proxy Server");
    a2->set_points(100);
    a2->set_completed(false);

    cout << course.code() << ": " << course.name() << endl;
    cout << "Assignments:" << endl;
    for (const auto& assignment : course.assignments()) {
        cout << "  - " << assignment.name()
             << " (" << assignment.points() << " pts)"
             << (assignment.completed() ? " [done]" : " [pending]")
             << endl;
    }
}

/* ============================================================================
 * EXERCISE 5: Enums
 * ============================================================================
 */

void exercise5_enums() {
    cout << "\n=== Exercise 5: Enums ===" << endl;

    FileInfo file;
    file.set_path("/home/user/document.txt");
    file.set_size(1024);
    file.set_type(FileInfo::REGULAR);

    cout << "Path: " << file.path() << endl;
    cout << "Size: " << file.size() << " bytes" << endl;

    // Switch on enum
    switch (file.type()) {
        case FileInfo::REGULAR:
            cout << "Type: Regular file" << endl;
            break;
        case FileInfo::DIRECTORY:
            cout << "Type: Directory" << endl;
            break;
        case FileInfo::SYMLINK:
            cout << "Type: Symbolic link" << endl;
            break;
        default:
            cout << "Type: Unknown" << endl;
    }

    // Get enum name as string
    cout << "Enum value: " << FileInfo::FileType_Name(file.type()) << endl;
}

/* ============================================================================
 * EXERCISE 6: Bytes field
 * ============================================================================
 */

void exercise6_bytes() {
    cout << "\n=== Exercise 6: Bytes Field ===" << endl;

    FileChunk chunk;

    // Binary data
    string binary_data = "\x00\x01\x02\x03\x04";
    binary_data += "Hello";  // Can mix with text

    chunk.set_data(binary_data);
    chunk.set_offset(0);
    chunk.set_is_last(true);

    cout << "Data size: " << chunk.data().size() << " bytes" << endl;
    cout << "Offset: " << chunk.offset() << endl;
    cout << "Is last: " << (chunk.is_last() ? "yes" : "no") << endl;

    // Access bytes
    const string& data = chunk.data();
    cout << "First 5 bytes (hex): ";
    for (int i = 0; i < 5 && i < (int)data.size(); i++) {
        printf("%02x ", (unsigned char)data[i]);
    }
    cout << endl;
}

/* ============================================================================
 * EXERCISE 7: Map field
 * ============================================================================
 */

void exercise7_maps() {
    cout << "\n=== Exercise 7: Map Field ===" << endl;

    FileCache cache;

    // Add entries to map
    FileInfo info1;
    info1.set_path("/file1.txt");
    info1.set_size(100);
    info1.set_type(FileInfo::REGULAR);

    (*cache.mutable_files())["/file1.txt"] = info1;

    // Alternative way
    auto& files = *cache.mutable_files();
    files["/file2.txt"].set_path("/file2.txt");
    files["/file2.txt"].set_size(200);

    // Access
    cout << "Cache contents:" << endl;
    for (const auto& [path, file] : cache.files()) {
        cout << "  " << path << ": " << file.size() << " bytes" << endl;
    }

    // Check if key exists
    if (cache.files().count("/file1.txt") > 0) {
        cout << "file1.txt is cached" << endl;
    }
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main() {
    // Verify protobuf library version
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    cout << "\n================================================" << endl;
    cout << "  Module 02: Protocol Buffers" << endl;
    cout << "================================================" << endl;

    exercise1_simple_message();
    exercise2_serialization();
    exercise3_repeated();
    exercise4_nested();
    exercise5_enums();
    exercise6_bytes();
    exercise7_maps();

    // Clean up protobuf library
    google::protobuf::ShutdownProtobufLibrary();

    cout << "\n================================================" << endl;
    cout << "  Protocol Buffers Complete!" << endl;
    cout << "  Key points:" << endl;
    cout << "  - Define messages in .proto files" << endl;
    cout << "  - Generate code with protoc" << endl;
    cout << "  - Efficient binary serialization" << endl;
    cout << "================================================\n" << endl;

    return 0;
}
