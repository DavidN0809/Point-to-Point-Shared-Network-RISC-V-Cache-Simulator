#ifndef RAM_H
#define RAM_H

#include <cstdint>
#include <unordered_map>
#include <iostream>
#include <ostream>

class Ram {
    std::unordered_map<uint64_t, uint64_t> memoryData;

public:
    Ram() {
        for (uint64_t address = 0; address < 4096; ++address) {
            memoryData[address] = 0; // Prefill with 0
        }
    }
    
    uint64_t read(uint64_t address) {
    // Check if the address is present in the memoryData map
    auto it = memoryData.find(address);
    if (it != memoryData.end()) {
        // Return the data associated with the address using the iterator
        std::cout << "RAM Read at Address: " << address << ", Data: " << it->second << std::endl;
        return it->second;
    } else {
        std::cerr << "Read from uninitialized memory address: " << std::hex << address << std::endl;
        return 1; // Return a distinct error value
    }
}


    void write(uint64_t address, uint64_t data) {
        std::cout << "RAM Write at Address: " << address << ", Data: " << data << std::endl;

        memoryData[address] = data;
    }
void printContents() const {
    std::cout << "Non-zero RAM Contents:" << std::endl;
    for (const auto& pair : memoryData) {
        if (pair.second != 0) {  // Check if the data is not zero
            std::cout << "Address " << pair.first << ": Data " << pair.second << std::endl;
        }
    }
}

};

#endif //RAM_H
