// message.h
#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <string>

// Message Types for Network Communication
enum class MessageType {
    ReadMiss,
    WriteMiss,
    DataWriteBack,
    Invalidate,
    DataValueReply,
    Fetch
};

// Structure for a message in the network
struct Message {
    MessageType type;
    uint64_t address;
    uint64_t data;  // Used for DataWriteBack and DataValueReply
    int sourceID;   // ID of the source node
    int destID;     // ID of the destination node

    Message(MessageType type, uint64_t addr, uint64_t data, int srcID, int dstID)
        : type(type), address(addr), data(data), sourceID(srcID), destID(dstID) {}
        
    std::string messageTypeToString() const {
        switch (type) {
            case MessageType::ReadMiss: return "ReadMiss";
            case MessageType::WriteMiss: return "WriteMiss";
            case MessageType::DataWriteBack: return "DataWriteBack";
            case MessageType::Invalidate: return "Invalidate";
            case MessageType::DataValueReply: return "DataValueReply";
            case MessageType::Fetch: return "Fetch";
            default: return "UnknownType";
        }
    }
};
#endif // MESSAGE_H

