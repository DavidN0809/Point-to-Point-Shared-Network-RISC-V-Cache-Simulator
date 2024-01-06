#ifndef NETWORK_H
#define NETWORK_H

#include <queue>
#include <vector>
#include <functional>
#include <cstdint>
#include "message.h"

// Forward declarations
class Cache;
class Directory;

// Forward declaration
class Cache;
class Directory;

// Network Node Class, ie single nodes, cache etc
class NetworkNode {
public:
    int id;
    Cache* cache;
    Directory* directory;
    std::function<void(Message)> sendMessage;

    NetworkNode(int id, Cache* cache, Directory* dir)
        : id(id), cache(cache), directory(dir) {}

    void receiveMessage(const Message& message);
};

// Network Class, routing for nodes
class Network {
    std::vector<NetworkNode> nodes;

public:
    void addNode(NetworkNode& node);
    void sendMessage(const Message& message);
    void routeMessage(const Message& message);
};

#endif // NETWORK_H

