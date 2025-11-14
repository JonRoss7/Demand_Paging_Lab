#ifndef CPU_H
#define CPU_H

#include "Process.h"

class CPU {
private:
    int cyclesExecuted;
    int threadSwitches;

public:
    CPU() : cyclesExecuted(0), threadSwitches(0) {}
    void execute(Process& p, int threadId);
    void printStats() const;
    void recordContextSwitch();
};
#endif