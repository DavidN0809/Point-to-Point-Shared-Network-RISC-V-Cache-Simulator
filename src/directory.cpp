#include "directory.h"

void Directory::updateEntry(uint64_t address, int cpuID, DirectoryState newState) {
    size_t index = address % numEntries;
    DirectoryEntry& entry = entries[index];

    entry.tag = address;
    entry.state = newState;
}

bool Directory::isCachedElsewhere(uint64_t address, int cpuID) {
    size_t index = address % numEntries;
    const DirectoryEntry& entry = entries[index];

    return (entry.tag == address && entry.state != DirectoryState::UNCACHED);
}

uint64_t Directory::readData(uint64_t address, int requestingCpuID, Ram& ram) {
    size_t index = address % numEntries;
    DirectoryEntry& entry = entries[index];

    if (entry.tag != address) {
        uint64_t data = ram.read(address);
        entry.tag = address;
        entry.state = DirectoryState::SHARED;
            std::cout << "RAM Read at Address: " << address << ", Data: " << data << std::endl;
        return data;
    }

    if (entry.state == DirectoryState::EXCLUSIVE) {
                // Fetch data from the owner node instead of memory
        // Assuming there is only one owner in EXCLUSIVE state
        int ownerID = *entry.sharers.begin(); // Get the owner's ID
        // Send a fetch request to the owner
        sendNetworkMessage(Message(MessageType::Fetch, address, 0, this->id, ownerID));
        // The actual data will be returned in response to the fetch request
        // Therefore, return a invalid data here
        return 0;
    } else if (entry.state == DirectoryState::SHARED) {
        std::cout << "RAM Read at Address: " << address << ", Data: " <<  ram.read(address) << std::endl;
        return ram.read(address);
    }

    return 0; // Indicative of an error or invalid state
}

void Directory::sendNetworkMessage(const Message& message) {
    network->sendMessage(message);
    Directory::printDirectoryEntryState(message.address);
}

void Directory::receiveMessage(const Message& message) {
    size_t index = message.address % numEntries;
    DirectoryEntry& entry = entries[index];
     std::cout << "Directory received message type: " << message.messageTypeToString() 
              << " for address: " << message.address << " from CPU" << message.sourceID 
              << ", Data: " << message.data << std::endl;


    switch (message.type) {
        case MessageType::ReadMiss:
            if (entry.state == DirectoryState::UNCACHED) {
                entry.state = DirectoryState::SHARED;
                entry.sharers.clear();
                entry.sharers.insert(message.sourceID);
                  // Read the data from RAM
            uint64_t data = ram.read(message.address);
            sendNetworkMessage(Message(MessageType::DataValueReply, message.address, data, this->id, message.sourceID));
   
            } else if (entry.state == DirectoryState::SHARED) {
                    entry.sharers.insert(message.sourceID);
                sendNetworkMessage(Message(MessageType::DataValueReply, message.address, ram.read(message.address), this->id, message.sourceID));
                
            } else if (entry.state == DirectoryState::EXCLUSIVE) {
                sendNetworkMessage(Message(MessageType::Fetch, message.address, 0, this->id, message.sourceID));
            }
            break;

        case MessageType::WriteMiss:
            if (entry.state == DirectoryState::SHARED) {
                // Send invalidate messages to all sharers except the requesting node
                for (int sharerID : entry.sharers) {
                    if (sharerID != message.sourceID) {
                        sendNetworkMessage(Message(MessageType::Invalidate, message.address, 0, sharerID, this->id));
                    }
                }
            }
            
            // For both UNCACHED and SHARED states
               entry.state = DirectoryState::EXCLUSIVE;
    entry.sharers.clear();
    entry.sharers.insert(message.sourceID);
            // Send the data value reply to the requesting node
    sendNetworkMessage(Message(MessageType::DataValueReply, message.address, message.data, this->id, message.sourceID));

            break;

        case MessageType::DataWriteBack:
            ram.write(message.address, message.data);
            entry.state = DirectoryState::UNCACHED;
            break;

        default:
            // Default action or unrecognized message type
            break;
    }
}


