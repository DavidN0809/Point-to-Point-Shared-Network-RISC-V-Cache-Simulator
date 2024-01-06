#include "network.h"
#include <iostream>
#include "cache.h"
#include "directory.h"
void NetworkNode::receiveMessage(const Message& message) {
    // Handle message based on its type
    switch (message.type) {
        case MessageType::ReadMiss:
        case MessageType::WriteMiss:
        case MessageType::Invalidate:
            // These messages are typically directed towards a cache
            if (cache) {
                cache->handleNetworkMessage(message);
            }
            break;
        case MessageType::DataWriteBack:
        case MessageType::DataValueReply:
        case MessageType::Fetch:
            // These messages are typically for the directory
            if (directory) {
                directory->receiveMessage(message);
            }
            break;
        default:
            // Default action or error handling
            std::cerr << "Unhandled message type received." << std::endl;
            break;
    }
}

// Network methods
void Network::addNode(NetworkNode& node) {
    node.sendMessage = [this](Message message) { this->sendMessage(message); };
    nodes.push_back(node);
}

void Network::sendMessage(const Message& message) {
    std::string messageType = message.messageTypeToString();
    std::cout << "Sending Message: Type=" << messageType 
              << ", Address=" << message.address << ", SourceID=" << message.sourceID 
              << ", DestID=" << message.destID << ", Data: " << message.data << std::endl;
    routeMessage(message);
}

void Network::routeMessage(const Message& message) {
    for (auto& node : nodes) {
        if (node.id == message.destID) {
            switch (message.type) {
                case MessageType::ReadMiss:
                case MessageType::WriteMiss:
                    // Route to the directory
                    if (node.directory) {
                        node.directory->receiveMessage(message);
                    }
                    break;

                case MessageType::DataWriteBack:
                    // Route DataWriteBack to the directory
                    if (node.directory) {
                        node.directory->receiveMessage(message);
                    }
                    break;

                case MessageType::Invalidate:
                case MessageType::Fetch:
                    // Route Invalidate and Fetch to the cache
                    if (node.cache) {
                        node.cache->handleNetworkMessage(message);
                    }
                    break;

                case MessageType::DataValueReply:
                    // Route DataValueReply to the cache
                    if (node.cache) {
                        node.cache->handleNetworkMessage(message);
                    }
                    break;

                default:
                    // Handle default case or error handling
                    std::cerr << "Unhandled message type received: " << message.messageTypeToString() << std::endl;
                    break;
            }
            break; // Break from the loop once the message is routed
        }
    }
}



