#include "simulation.h"

// Function to print the events
void Simulation::printEvents() {
    // std::cout << "Header";
    while (!systemEvents.empty()) {
        systemEvents.front().print();
        systemEvents.pop();
    }
}
void Simulation::runSimulation() {
    std::cout << "Begin System Initialization\n";
    srand(time(NULL));

    int numCPU = 4;
    const size_t numEntries = 4096; // Total number of cache lines

std::cout << "========================Create Caches and Directory=======================\n";
Network network;
std::vector<Directory> directories;
std::vector<Cache> caches;
std::vector<NetworkNode> networkNodes;

// Initialize directories
for (int i = 0; i < numCPU; ++i) {
    directories.emplace_back(numEntries, &network, mainMemory, i);
}

// Initialize caches
for (int i = 0; i < numCPU; ++i) {
    caches.emplace_back(i, mainMemory, directories[i], nullptr);
}

// Initialize network nodes
for (int i = 0; i < numCPU; ++i) {
    networkNodes.emplace_back(i, &caches[i], &directories[i]);
    caches[i].setNetworkNode(&networkNodes[i]);
    network.addNode(networkNodes[i]);
}

    // Simulation loop
    int loopCnt = 1;
    int loopMax = 1500;
    
    
    // Predefined sequences for specific transitions with descriptions
std::vector<std::tuple<int, uint64_t, int, bool, std::string>> transitions = {
    {2, 10, 0, false, "Write Miss on Exclusive Block"},
    {3, 11, 0, true, "Read Miss on Modified Block"},
   // {4, 12, 150, false, "Invalid to Exclusive due to CPU Write"},
    {0, 13, 250, false, "Shared to Invalid due to Other CPU Write"},
    {1, 14, 0, false, "Modified to Invalid (Direct Invalidation)"},
   // {2, 15, 0, false, "Exclusive to Invalid (Direct Invalidation)"},
    {0, 0, 200, true, "Invalid to Shared due to CPU Read"},
    {1, 0, 100, false, "Shared to Modified due to CPU Write"},
    {2, 1, 0, false, "Invalid to Modified due to CPU Write"},
    {3, 2, 0, true, "Shared to Invalid due to Invalidate"},
    {1, 3, 0, true, "Modified to Shared due to Read Miss"},
    {0, 4, 0, true, "Exclusive to Shared due to Read Miss"},
    // Add more transitions as needed...
};

    std::cout << "======================Simulation Begin======================\n";
   // while (loopCnt <= loopMax) {
        std::cout << "\n=====================Simulation Loop #" << loopCnt++ << "=====================\n";

        // Simulate processor read and write requests
        for (int i = 0; i < numCPU; ++i) {
       
    for (const auto& [cpuId, address, data, isRead, description] : transitions) {
        std::cout << "\nExpected Transition: " << description << std::endl;

        if (isRead) {
            std::cout << "CPU" << cpuId << " Reads from Address: " << address << "\n";
            caches[cpuId].readFromCache(address, [cpuId](uint64_t data) {
                std::cout << "CPU" << cpuId << " Received Data: " << data << std::endl;
            });
        } else {
            std::cout << "CPU" << cpuId << " Writes to Address: " << address << " with Data: " << data << "\n";
            caches[cpuId].writeToCache(address, data);
        }

        // Print the state of the current address in all caches
        for (int j = 0; j < numCPU; ++j) {
            caches[j].printCacheLineState(address, j);
        }


        // Increment simulation time
        currentTick += 10;
        systemEvents.push(Event(currentTick));
    	
    	}
    }
    std::cout << "\n=======================Simulation Ended=======================\n";
    if (loopCnt >= loopMax)
        std::cout << "=====Reason for Termination: Maximum loop counter reached=====\n"; 
    mainMemory.printContents();
        
}

// every possible transation for both state machines, 
// interface with bus and processors

// print out cycle number, state of queue, processor each cycle


// output:
// Cycle #
// Access Request queue
