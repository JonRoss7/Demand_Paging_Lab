#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <map>
#include <memory> 

#include "TwoLevelPageTable.h"
#include "TLB.h"
#include "Clock.h"
#include "Scheduler.h"
#include "Process.h"
#include "CPU.h"

#include "TwoLevelPageTable.cpp"
#include "TLB.cpp"
#include "Clock.cpp"
#include "Scheduler.cpp"
#include "Process.cpp"
#include "CPU.cpp"


const int OFFSET_BITS = 12;
int getPageNumFromAddress(unsigned int address) {
    return address >> OFFSET_BITS;
}

unsigned int hex_to_int(std::string hex_str) {
    if (hex_str.rfind("0x", 0) == 0) {
        hex_str = hex_str.substr(2);
    }
    unsigned int val;
    std::stringstream ss;
    ss << hex_str;
    ss >> std::hex >> val;
    return val;
}

struct TraceLine {
    int pid;
    int threadId;
    unsigned int address;
    bool isWrite;
};

struct Thread {
    int pid;
    int threadId;
    int currentQueue;
    int quantumRemaining;
    int waitTime;

    std::queue<TraceLine> myAccesses;

    Thread(int p, int t, int q) :
        pid(p), threadId(t), currentQueue(0), quantumRemaining(q), waitTime(0) {}
};


int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "Error: Missing arguments." << std::endl;
        std::cerr << "Usage: ./main <tlb_size> <frame_count>" << std::endl;
        return 1;
    }

    int tlbSize = std::atoi(argv[1]);
    int frameCount = std::atoi(argv[2]);
    std::string traceFileName = "input.txt";

    if (tlbSize <= 0 || frameCount <= 0) {
        std::cerr << "Error: TLB size and frame count must be positive integers." << std::endl;
        return 1;
    }

    std::ofstream logFile("events.log");
    if (!logFile) {
        std::cerr << "Error: Could not open log file: events.log" << std::endl;
        return 1;
    }
    logFile << "Time,Event,PID,ThreadID,Page,Frame,EvictedPage\n";


    std::cout << "--- Simulation starting with TLB Size: " << tlbSize 
              << ", Frame Count: " << frameCount << " ---" << std::endl;

    TwoLevelPageTable pageTable(frameCount);
    TLB tlb(tlbSize);
    Clock clock(frameCount);
    CPU cpu;
    Scheduler scheduler(SchedulingPolicy::MLFQ, tlbSize);
    
    std::ifstream traceFile(traceFileName);
    if (!traceFile) {
        std::cerr << "Error: Could not open trace file: " << traceFileName << std::endl;
        return 1;
    }

    const int QUANTA[] = {4, 8, 16};
    const int AGING_LIMIT = 50;

    std::vector<std::queue<std::shared_ptr<Thread>>> mlfq(3);
    std::map<std::pair<int, int>, std::shared_ptr<Thread>> threadMap;

    std::string line;
    while (std::getline(traceFile, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        int pid, tid;
        std::string addr_s, type_s;
        if (!(ss >> pid >> tid >> addr_s >> type_s)) continue;

        TraceLine access = {pid, tid, hex_to_int(addr_s), (type_s == "W")};
        std::pair<int, int> threadKey = {pid, tid};

        if (threadMap.find(threadKey) == threadMap.end()) {
            scheduler.addProcessIfNotExist(pid);
            auto newThread = std::make_shared<Thread>(pid, tid, QUANTA[0]);
            threadMap[threadKey] = newThread;
            mlfq[0].push(newThread);
        }
        threadMap[threadKey]->myAccesses.push(access);
    }
    traceFile.close();

    std::cout << "Read " << threadMap.size() << " unique threads from trace file." << std::endl;

    int simulationTime = 0;
    int currentThreadPid = -1;
    int currentThreadId = -1;

    while (true) {
        bool allQueuesEmpty = true;
        for (const auto& q : mlfq) {
            if (!q.empty()) {
                allQueuesEmpty = false;
                break;
            }
        }
        if (allQueuesEmpty) {
            break;
        }

        for (int q_level = 1; q_level < mlfq.size(); ++q_level) {
            int q_size = mlfq[q_level].size();
            for (int i = 0; i < q_size; ++i) {
                auto t = mlfq[q_level].front();
                mlfq[q_level].pop();
                t->waitTime++;
                if (t->waitTime > AGING_LIMIT) {
                    t->waitTime = 0;
                    t->currentQueue = q_level - 1;
                    t->quantumRemaining = QUANTA[t->currentQueue];
                    mlfq[t->currentQueue].push(t);
                    std::cout << "-> AGING: Promoted PID " << t->pid << ", Thread " << t->threadId << " to Q" << t->currentQueue << std::endl;
                } else {
                    mlfq[q_level].push(t);
                }
            }
        }

        std::shared_ptr<Thread> currentThread = nullptr;
        for (int i = 0; i < mlfq.size(); ++i) {
            if (!mlfq[i].empty()) {
                currentThread = mlfq[i].front();
                mlfq[i].pop();
                break;
            }
        }

        if (currentThread == nullptr) {
            simulationTime++;
            continue;
        }

        if (currentThread->myAccesses.empty()) {
            continue;
        }

        if (currentThread->pid != currentThreadPid || currentThread->threadId != currentThreadId) {
            cpu.recordContextSwitch();
            logFile << simulationTime << ",CONTEXT_SWITCH," << currentThread->pid << "," << currentThread->threadId << ",,,\n";
            currentThreadPid = currentThread->pid;
            currentThreadId = currentThread->threadId;
        }
        
        Process& process = scheduler.getProcess(currentThread->pid);
        TraceLine access = currentThread->myAccesses.front();
        currentThread->myAccesses.pop();
        
        int pageNum = getPageNumFromAddress(access.address);
        bool isWrite = access.isWrite;
        int frameNum = -1;

        std::cout << "\nTime " << simulationTime << ": [Q" << currentThread->currentQueue << "] "
                  << "PID " << currentThread->pid << ", Thread " << currentThread->threadId 
                  << " accessing Addr " << std::hex << access.address 
                  << " (Page " << std::dec << pageNum << "), Type: " << (isWrite ? "Write" : "Read") << std::endl;
        
        cpu.execute(process, currentThread->threadId);
        logFile << simulationTime << ",CPU_EXECUTE," << currentThread->pid << "," << currentThread->threadId << "," << pageNum << ",,\n";

        
        if (tlb.lookup(currentThread->pid, pageNum, frameNum)) {
            std::cout << "-> TLB Hit! Frame: " << frameNum << std::endl;
            logFile << simulationTime << ",TLB_HIT," << currentThread->pid << "," << currentThread->threadId << "," << pageNum << "," << frameNum << ",\n";
            clock.setReferenced(frameNum);
            clock.setModified(frameNum, isWrite);
        
        } else {
            std::cout << "-> TLB Miss." << std::endl;
            logFile << simulationTime << ",TLB_MISS," << currentThread->pid << "," << currentThread->threadId << "," << pageNum << ",,\n";
            frameNum = pageTable.getFrame(currentThread->pid, pageNum);

            if (frameNum != -1) {
                std::cout << "-> Page Table Hit. Frame: " << frameNum << std::endl;
                clock.setReferenced(frameNum);
                clock.setModified(frameNum, isWrite);
                tlb.insert(currentThread->pid, pageNum, frameNum);
            } else {
                std::cout << "-> Page Fault!" << std::endl;
                logFile << simulationTime << ",PAGE_FAULT," << currentThread->pid << "," << currentThread->threadId << "," << pageNum << ",,\n";
                int evictedPage = -1;
                bool isEvictedPageModified = false;
                
                frameNum = clock.evictAndReplace(
                    currentThread->pid, pageNum, isWrite, 
                    evictedPage, isEvictedPageModified
                );

                std::cout << "-> Page loaded into Frame " << frameNum << "." << std::endl;
                logFile << simulationTime << ",PAGE_LOAD," << currentThread->pid << "," << currentThread->threadId << "," << pageNum << "," << frameNum << "," << evictedPage << "\n";
                
                if (evictedPage != -1) {
                    std::cout << "-> Evicted Page " << evictedPage << ". " 
                              << (isEvictedPageModified ? "(Dirty)" : "(Clean)") << std::endl;
                }
                pageTable.loadPage(currentThread->pid, pageNum, frameNum);
                tlb.insert(currentThread->pid, pageNum, frameNum);
            }
        }

        currentThread->quantumRemaining--;
        currentThread->waitTime = 0;

        if (currentThread->myAccesses.empty()) {
            std::cout << "-> Thread " << currentThread->pid << ":" << currentThread->threadId << " finished all accesses." << std::endl;
        } else if (currentThread->quantumRemaining <= 0) {
            int nextQueue = std::min(currentThread->currentQueue + 1, (int)mlfq.size() - 1);
            currentThread->currentQueue = nextQueue;
            currentThread->quantumRemaining = QUANTA[nextQueue];
            mlfq[nextQueue].push(currentThread);
            std::cout << "-> Quantum expired. Demoting PID " << currentThread->pid << ", Thread " << currentThread->threadId << " to Q" << nextQueue << std::endl;
        } else {
            mlfq[currentThread->currentQueue].push(currentThread);
        }

        simulationTime++;
    }

    logFile.close();
    std::cout << "\n\n--- Final Simulation Stats ---" << std::endl;
    tlb.printStats();
    std::cout << "--------------------" << std::endl;
    pageTable.printStats();
    std::cout << "--------------------" << std::endl;
    cpu.printStats();

    return 0;
}
