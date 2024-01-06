#ifndef SIMULATION_H
#define SIMULATION_H

#include <queue>
#include <cstdlib>
#include "directory.h"

#include "event.h"
#include "cache.h"
#include "ram.h"

class Simulation {
    std::queue<Event> systemEvents;
    tick_t currentTick;
    Ram mainMemory;

public:
    // Function to run the simulation
    void printEvents();
    void runSimulation();
    
};

#endif //SIMULATION_H
