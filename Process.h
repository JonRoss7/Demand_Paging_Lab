#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <map>
#include <utility>

class Process {
public:
    int pid;
    int priority;
    int cpuBurstRemaining;
    std::vector<std::pair<int, unsigned int>> memoryAccesses;
    size_t accessIndex;
    std::map<int, int> threadPriorities;

    Process(int pid, int priority);
    bool hasMoreAccesses() const;
    std::pair<int, unsigned int> nextAccess();
    void addThread(int threadId, int threadPriority);
    void tick();
};

#endif