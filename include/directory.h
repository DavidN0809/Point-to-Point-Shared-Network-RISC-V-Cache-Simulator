#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <vector>
#include <bitset>
#include "cachestate.h"
#include "ram.h"
#include "network.h"  
#include "directorystate.h" 
#include <set> 

class Cache;  // Forward declaration of Cache

struct DirectoryEntry {
    uint32_t tag;  // Tag to identify the cache line
    DirectoryState state;  // Directory entry state
    std::set<int> sharers; 
    DirectoryEntry() : tag(0), state(DirectoryState::UNCACHED) {}
};

class Directory {
    size_t numEntries;
    std::vector<DirectoryEntry> entries;
    Network* network;
    Ram& ram;
    int id; // Identifier for the directory ( corresponding to a CPU)
      
public:
    Directory(size_t numEntries, Network* net, Ram& ram, int id) : 
        numEntries(numEntries), entries(numEntries), network(net), ram(ram), id(id) {}

    DirectoryEntry& getEntry(size_t index) { return entries.at(index); }
    const DirectoryEntry& getEntry(size_t index) const { return entries.at(index); }

    // Methods for updating and querying directory entries
    void updateEntry(uint64_t address, int cpuID, DirectoryState newState);
    bool isCachedElsewhere(uint64_t address, int cpuID);
    uint64_t readData(uint64_t address, int requestingCpuID, Ram& ram);

    void sendNetworkMessage(const Message& message);  // Method to send network messages
    void receiveMessage(const Message& message);
         size_t getNumEntries() const { return numEntries; }
         
    void printDirectoryEntryState(uint64_t address) const {
        size_t index = address % numEntries;
        const DirectoryEntry& entry = entries[index];

        std::cout << "Directory (" << id << ") Entry State: Address=" << address 
                  << ", State=" << toString(entry.state) << std::endl;
    std::cout << "Sharers: ";
    for (int sharer : entries[index].sharers) {
        std::cout << sharer << " ";
    }
    std::cout << std::endl;
}

    // Helper method to convert state enum to string
    std::string toString(DirectoryState state) const {
        switch (state) {
            case DirectoryState::UNCACHED: return "UNCACHED";
            case DirectoryState::SHARED: return "SHARED";
            case DirectoryState::EXCLUSIVE: return "EXCLUSIVE";
            default: return "UNKNOWN";
        }
    }
};

#endif // DIRECTORY_H

