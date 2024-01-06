#include "simulation.h"
#include <fstream>

int main() {
    std::ofstream outputFile("output.txt");
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open the output file." << std::endl;
        return 0; // Return an error code
    }
    
    std::streambuf *coutBuffer = std::cout.rdbuf(); // Save the original cout buffer
    std::cout.rdbuf(outputFile.rdbuf()); // Redirect cout to the output file stream

    //*******************************************************************************
    Simulation test;        // Create a simulation object
    test.runSimulation();   // Run the simulation
    //*******************************************************************************

    std::cout.rdbuf(coutBuffer); // Restore the original cout buffer
    outputFile.close();

    return 0;
}