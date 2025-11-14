#include "Process.h"

Process::Process(int pid, int priority) : 
    pid(pid), 
    priority(priority), 
    cpuBurstRemaining(0),
    accessIndex(0) {
}

bool Process::hasMoreAccesses() const {
    return accessIndex < memoryAccesses.size();
}

std::pair<int, unsigned int> Process::nextAccess() {
    if (hasMoreAccesses()) {
        return memoryAccesses[accessIndex++];
    }
    return {-1, 0}; 
}

void Process::addThread(int threadId, int threadPriority) {
    threadPriorities[threadId] = threadPriority;
}

void Process::tick() {
    if (cpuBurstRemaining > 0) {
        cpuBurstRemaining--;
    }
}
