#include "cache.h"
#include "directory.h" 
#include <thread> // For std::this_thread
#include <chrono> // For std::chrono::milliseconds
CacheState Cache::getCurrentState(uint64_t address)
{
    auto it = cacheData.find(address);

    if (it != cacheData.end()) {
        return it->second.state;
    } else {
        return CacheState::INVALID;
    }
}

void Cache::setCurrentState(uint64_t address, CacheState newState) {
    auto it = cacheData.find(address);

    if (it != cacheData.end()) {
        it->second.state = newState;
    }
      //  printCacheLineState(address); // Display the state of the cache line
}

uint64_t Cache::readDataDirectly(uint64_t address) {
    auto it = cacheData.find(address);
    if (it != cacheData.end()) {
        return it->second.data;
    } else {
        // Handle cache miss or return a default value
        return 0;
    }
}

    // New method to handle incoming data (to be called from handleNetworkMessage)
void Cache::onDataReceived(uint64_t address, uint64_t data) {
    auto it = cacheData.find(address);
    if (it != cacheData.end()) {
        it->second.data = data; // Update data in cache line
        it->second.state = CacheState::MODIFIED; // Update state to MODIFIED
    } else {
        // If it's not in the cache, add it
        cacheData[address] = {CacheState::MODIFIED, address, data};
    }

    auto readIt = pendingReads.find(address);
    if (readIt != pendingReads.end()) {
        readIt->second.callback(data); // Invoke the callback
        pendingReads.erase(readIt); // Remove the pending request
    }
}

void Cache::readFromCache(uint64_t address, std::function<void(uint64_t)> callback) {
    auto it = cacheData.find(address);
    if (it != cacheData.end() && it->second.state != CacheState::INVALID) {
            std::cout << "CPU" << id << " Reads from Address: " << address << " Data: " << it->second.data << std::endl;
        callback(it->second.data); // Immediate callback on cache hit
    } else {
        // Cache miss logic
       Message readMissMsg(MessageType::ReadMiss, address, 0, this->id,  this->id);
        directory.sendNetworkMessage(readMissMsg);
        pendingReads[address] = {callback}; // Store the callback for later invocation
    }
}


void Cache::writeToCache(uint64_t address, uint64_t data) {
    auto it = cacheData.find(address);
    if (it != cacheData.end() && it->second.state == CacheState::MODIFIED) {
        it->second.data = data;  // Update the data directly in the cache
    } else {
        Message writeMissMsg(MessageType::WriteMiss, address, data, this->id, this->id);
        directory.sendNetworkMessage(writeMissMsg);
        // Update the cache line state as needed
        setCurrentState(address, CacheState::MODIFIED);
    }
}


void Cache::printCacheLineState(uint64_t address, int cpuID) const {
    auto it = cacheData.find(address);
    if (it != cacheData.end()) {
        std::cout << "CPU" << cpuID << " Address: " << address << " State: ";
        switch (it->second.state) {
            case CacheState::MODIFIED: std::cout << "M"; break;
            case CacheState::SHARED: std::cout << "S"; break;
            case CacheState::INVALID: std::cout << "I"; break;
            default: std::cout << "Unknown";
        }
        // Additionally print the data stored at this cache line
        std::cout << " Data: " << it->second.data << std::endl;
    } else {
        std::cout << "CPU" << cpuID << " Address: " << address << " State: Not in Cache" << std::endl;
    }
}
void Cache::handleNetworkMessage(const Message& message) {
 std::cout << "CPU" << id << " handling message type: " << message.messageTypeToString() 
              << " for address: " << message.address << ", Data: " << message.data << std::endl;

    switch (message.type) {
        case MessageType::ReadMiss:
            // Handle ReadMiss message
            if (getCurrentState(message.address) == CacheState::INVALID) {
                setCurrentState(message.address, CacheState::SHARED);
                onDataReceived(message.address, message.data);
            }
            
            break;

        case MessageType::WriteMiss:
            // Handle WriteMiss message
            setCurrentState(message.address, CacheState::MODIFIED);
            onDataReceived(message.address, message.data);
            break;

        case MessageType::Fetch:
            // Handle Fetch message
            if (getCurrentState(message.address) == CacheState::MODIFIED) {
                uint64_t data = readDataDirectly(message.address);
                networkNode->sendMessage(Message(MessageType::DataValueReply, message.address, data, id, message.destID));
                setCurrentState(message.address, CacheState::SHARED);
            }
            break;

        case MessageType::DataWriteBack:
            // Handle DataWriteBack message
    if (getCurrentState(message.address) == CacheState::MODIFIED) {
        ram.write(message.address, readDataDirectly(message.address));
        setCurrentState(message.address, CacheState::INVALID);
    }
            break;

        case MessageType::Invalidate:
            // Handle Invalidate message
                setCurrentState(message.address, CacheState::INVALID);
            break;

        case MessageType::DataValueReply:
    // Update the cache line with new data and state
    auto it = cacheData.find(message.address);
    if (it != cacheData.end()) {
        it->second.data = message.data; // Update the existing cache line's data
        it->second.state = CacheState::SHARED; // Update the state
    } else {
        // If the cache line doesn't exist, create a new one
        cacheData[message.address] = {CacheState::SHARED, message.address, message.data};
    }

    // Debug statement to print the received data
    std::cout << "CPU" << id << " Received DataValueReply for Address: " << message.address 
              << " Data: " << message.data << std::endl;

            break;

    }
}


