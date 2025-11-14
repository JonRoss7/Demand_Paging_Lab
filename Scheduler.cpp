#include "Scheduler.h"
#include <iostream>

Scheduler::Scheduler(SchedulingPolicy policy, int tlbSize) : policy(policy) {
    if (policy == SchedulingPolicy::MLFQ) {
        readyQueues.resize(3); 
    }
}

void Scheduler::addProcessIfNotExist(int pid) {
    if (processes.find(pid) == processes.end()) {
        processes.emplace(pid, Process(pid, 0));
        
        if (policy == SchedulingPolicy::MLFQ) {
            readyQueues[0].push(pid);
        }
    }
}

Process& Scheduler::getProcess(int pid) {
    return processes.at(pid);
}

void Scheduler::tick() {
}

void Scheduler::adjustPriority(int pid, int threadId) {
}