#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <string>

typedef uint64_t tick_t;

class Event {
    tick_t simTick;
    
public:
    Event(tick_t clk) : simTick(clk) {}

    tick_t getTime() { return simTick; }

    void print() {
        std::cout << "****************************************\n" 
        << "Clock ticks: " << simTick << "\n" 

        << "****************************************\n";
    }
};

#endif //EVENT_H