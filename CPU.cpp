#include "CPU.h"
#include <iostream>

void CPU::execute(Process& p, int threadId) {
    cyclesExecuted++;
}

void CPU::recordContextSwitch() {
    threadSwitches++;
}

void CPU::printStats() const {
    std::cout << "--- CPU Stats ---" << std::endl;
    std::cout << "Total cycles executed: " << cyclesExecuted << std::endl;
    std::cout << "Total thread/context switches: " << threadSwitches << std::endl;
}