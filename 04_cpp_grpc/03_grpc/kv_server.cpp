/*
 * CS-6200 Preparation - Module 03: gRPC Server
 *
 * Key-value store server implementation.
 *
 * Build: cmake .. && make (from build directory)
 * Run:   ./kv_server
 *
 * Difficulty: [HARD]
 */

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

#include <grpcpp/grpcpp.h>
#include "keyvalue.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
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
 * KEY-VALUE STORE SERVICE IMPLEMENTATION
 * ============================================================================
 */

class KeyValueStoreImpl final : public KeyValueStore::Service {
private:
    std::unordered_map<std::string, std::string> store_;
    std::mutex mutex_;  // Thread-safe access

public:
    /*
     * Unary RPC: Get a value by key
     */
    Status Get(ServerContext* context, const GetRequest* request,
               GetResponse* response) override {
        (void)context;  // Unused

        std::lock_guard<std::mutex> lock(mutex_);

        std::cout << "GET: " << request->key() << std::endl;

        auto it = store_.find(request->key());
        if (it != store_.end()) {
            response->set_found(true);
            response->set_value(it->second);
        } else {
            response->set_found(false);
        }

        return Status::OK;
    }

    /*
     * Unary RPC: Put a key-value pair
     */
    Status Put(ServerContext* context, const PutRequest* request,
               PutResponse* response) override {
        (void)context;

        std::lock_guard<std::mutex> lock(mutex_);

        std::cout << "PUT: " << request->key() << " = " << request->value() << std::endl;

        bool existed = store_.count(request->key()) > 0;
        store_[request->key()] = request->value();

        response->set_success(true);
        response->set_was_update(existed);

        return Status::OK;
    }

    /*
     * Unary RPC: Delete a key
     */
    Status Delete(ServerContext* context, const DeleteRequest* request,
                  DeleteResponse* response) override {
        (void)context;

        std::lock_guard<std::mutex> lock(mutex_);

        std::cout << "DELETE: " << request->key() << std::endl;

        auto it = store_.find(request->key());
        if (it != store_.end()) {
            store_.erase(it);
            response->set_success(true);
            response->set_existed(true);
        } else {
            response->set_success(true);
            response->set_existed(false);
        }

        return Status::OK;
    }

    /*
     * Server streaming RPC: List all keys
     */
    Status ListKeys(ServerContext* context, const ListKeysRequest* request,
                    ServerWriter<KeyResponse>* writer) override {
        (void)context;

        std::lock_guard<std::mutex> lock(mutex_);

        std::cout << "LIST_KEYS: prefix='" << request->prefix() << "'" << std::endl;

        const std::string& prefix = request->prefix();

        for (const auto& [key, value] : store_) {
            // Filter by prefix if specified
            if (prefix.empty() || key.substr(0, prefix.size()) == prefix) {
                KeyResponse response;
                response.set_key(key);

                if (!writer->Write(response)) {
                    // Client disconnected
                    return Status::CANCELLED;
                }
            }
        }

        return Status::OK;
    }

    /*
     * Client streaming RPC: Put multiple key-value pairs
     */
    Status PutMultiple(ServerContext* context,
                       ServerReader<PutRequest>* reader,
                       PutMultipleResponse* response) override {
        (void)context;

        std::lock_guard<std::mutex> lock(mutex_);

        std::cout << "PUT_MULTIPLE: starting" << std::endl;

        PutRequest request;
        int count = 0;

        while (reader->Read(&request)) {
            store_[request.key()] = request.value();
            count++;
            std::cout << "  PUT: " << request.key() << std::endl;
        }

        std::cout << "PUT_MULTIPLE: inserted " << count << " keys" << std::endl;

        response->set_count(count);
        return Status::OK;
    }
};

/* ============================================================================
 * MAIN - Start the server
 * ============================================================================
 */

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    KeyValueStoreImpl service;

    ServerBuilder builder;

    // Listen on the given address without authentication
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    // Register our service
    builder.RegisterService(&service);

    // Build and start the server
    std::unique_ptr<Server> server(builder.BuildAndStart());

    std::cout << "================================================" << std::endl;
    std::cout << "  Key-Value Store Server" << std::endl;
    std::cout << "  Listening on " << server_address << std::endl;
    std::cout << "  Press Ctrl+C to stop" << std::endl;
    std::cout << "================================================" << std::endl;

    // Wait for the server to shutdown
    server->Wait();
}

int main() {
    RunServer();
    return 0;
}
