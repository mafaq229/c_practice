/*
 * CS-6200 Preparation - Module 03: gRPC Client
 *
 * Key-value store client implementation.
 *
 * Build: cmake .. && make (from build directory)
 * Run:   ./kv_client (make sure kv_server is running)
 *
 * Difficulty: [HARD]
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>
#include "keyvalue.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientWriter;
using grpc::Status;

using keyvalue::KeyValueStore;
using keyvalue::GetRequest;
using keyvalue::GetResponse;
using keyvalue::PutRequest;
using keyvalue::PutResponse;
using keyvalue::DeleteRequest;
using keyvalue::DeleteResponse;
using keyvalue::ListKeysRequest;
using keyvalue::KeyResponse;
using keyvalue::PutMultipleResponse;

/* ============================================================================
 * KEY-VALUE STORE CLIENT
 * ============================================================================
 */

class KeyValueClient {
private:
    std::unique_ptr<KeyValueStore::Stub> stub_;

public:
    KeyValueClient(std::shared_ptr<Channel> channel)
        : stub_(KeyValueStore::NewStub(channel)) {}

    /*
     * Unary RPC: Get a value by key
     */
    bool Get(const std::string& key, std::string& value) {
        GetRequest request;
        request.set_key(key);

        GetResponse response;
        ClientContext context;

        Status status = stub_->Get(&context, request, &response);

        if (!status.ok()) {
            std::cerr << "RPC failed: " << status.error_message() << std::endl;
            return false;
        }

        if (response.found()) {
            value = response.value();
            return true;
        }
        return false;
    }

    /*
     * Unary RPC: Put a key-value pair
     */
    bool Put(const std::string& key, const std::string& value) {
        PutRequest request;
        request.set_key(key);
        request.set_value(value);

        PutResponse response;
        ClientContext context;

        Status status = stub_->Put(&context, request, &response);

        if (!status.ok()) {
            std::cerr << "RPC failed: " << status.error_message() << std::endl;
            return false;
        }

        return response.success();
    }

    /*
     * Unary RPC: Delete a key
     */
    bool Delete(const std::string& key, bool& existed) {
        DeleteRequest request;
        request.set_key(key);

        DeleteResponse response;
        ClientContext context;

        Status status = stub_->Delete(&context, request, &response);

        if (!status.ok()) {
            std::cerr << "RPC failed: " << status.error_message() << std::endl;
            return false;
        }

        existed = response.existed();
        return response.success();
    }

    /*
     * Server streaming RPC: List all keys
     */
    std::vector<std::string> ListKeys(const std::string& prefix = "") {
        ListKeysRequest request;
        request.set_prefix(prefix);

        ClientContext context;

        // Get the reader for streaming responses
        std::unique_ptr<ClientReader<KeyResponse>> reader(
            stub_->ListKeys(&context, request));

        std::vector<std::string> keys;
        KeyResponse response;

        while (reader->Read(&response)) {
            keys.push_back(response.key());
        }

        Status status = reader->Finish();
        if (!status.ok()) {
            std::cerr << "ListKeys failed: " << status.error_message() << std::endl;
        }

        return keys;
    }

    /*
     * Client streaming RPC: Put multiple key-value pairs
     */
    int PutMultiple(const std::vector<std::pair<std::string, std::string>>& pairs) {
        PutMultipleResponse response;
        ClientContext context;

        // Get the writer for streaming requests
        std::unique_ptr<ClientWriter<PutRequest>> writer(
            stub_->PutMultiple(&context, &response));

        for (const auto& [key, value] : pairs) {
            PutRequest request;
            request.set_key(key);
            request.set_value(value);

            if (!writer->Write(request)) {
                // Connection broken
                break;
            }
        }

        writer->WritesDone();
        Status status = writer->Finish();

        if (!status.ok()) {
            std::cerr << "PutMultiple failed: " << status.error_message() << std::endl;
            return -1;
        }

        return response.count();
    }
};

/* ============================================================================
 * TEST FUNCTIONS
 * ============================================================================
 */

void test_basic_operations(KeyValueClient& client) {
    std::cout << "\n=== Test: Basic Operations ===" << std::endl;

    // Put some values
    std::cout << "Putting values..." << std::endl;
    client.Put("name", "Alice");
    client.Put("age", "25");
    client.Put("city", "Atlanta");

    // Get values
    std::cout << "Getting values..." << std::endl;
    std::string value;

    if (client.Get("name", value)) {
        std::cout << "  name = " << value << std::endl;
    }

    if (client.Get("age", value)) {
        std::cout << "  age = " << value << std::endl;
    }

    if (!client.Get("nonexistent", value)) {
        std::cout << "  nonexistent: not found" << std::endl;
    }

    // Delete
    std::cout << "Deleting 'city'..." << std::endl;
    bool existed;
    client.Delete("city", existed);
    std::cout << "  existed before delete: " << (existed ? "yes" : "no") << std::endl;
}

void test_list_keys(KeyValueClient& client) {
    std::cout << "\n=== Test: List Keys ===" << std::endl;

    // Add some keys with a prefix
    client.Put("user:1", "Alice");
    client.Put("user:2", "Bob");
    client.Put("user:3", "Charlie");
    client.Put("item:1", "Apple");
    client.Put("item:2", "Banana");

    // List all keys
    std::cout << "All keys:" << std::endl;
    auto keys = client.ListKeys();
    for (const auto& key : keys) {
        std::cout << "  " << key << std::endl;
    }

    // List with prefix
    std::cout << "\nKeys with prefix 'user:':" << std::endl;
    keys = client.ListKeys("user:");
    for (const auto& key : keys) {
        std::cout << "  " << key << std::endl;
    }
}

void test_put_multiple(KeyValueClient& client) {
    std::cout << "\n=== Test: Put Multiple (Client Streaming) ===" << std::endl;

    std::vector<std::pair<std::string, std::string>> pairs = {
        {"batch:1", "value1"},
        {"batch:2", "value2"},
        {"batch:3", "value3"},
        {"batch:4", "value4"},
        {"batch:5", "value5"},
    };

    std::cout << "Sending " << pairs.size() << " key-value pairs..." << std::endl;
    int count = client.PutMultiple(pairs);
    std::cout << "Server confirmed: " << count << " inserted" << std::endl;

    // Verify
    std::string value;
    if (client.Get("batch:3", value)) {
        std::cout << "Verification: batch:3 = " << value << std::endl;
    }
}

/* ============================================================================
 * MAIN
 * ============================================================================
 */

int main() {
    std::cout << "================================================" << std::endl;
    std::cout << "  Key-Value Store Client" << std::endl;
    std::cout << "  Connecting to localhost:50051" << std::endl;
    std::cout << "================================================" << std::endl;

    // Create a channel to the server
    auto channel = grpc::CreateChannel(
        "localhost:50051",
        grpc::InsecureChannelCredentials()
    );

    KeyValueClient client(channel);

    // Run tests
    test_basic_operations(client);
    test_list_keys(client);
    test_put_multiple(client);

    std::cout << "\n================================================" << std::endl;
    std::cout << "  Client Complete!" << std::endl;
    std::cout << "================================================\n" << std::endl;

    return 0;
}
