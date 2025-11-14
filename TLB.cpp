#include "TLB.h"
#include <iostream>
#include <limits>

TLB::TLB(int size) : maxSize(size), currentTime(0), hits(0), misses(0) {
    tlbEntries.reserve(maxSize);
}

bool TLB::lookup(int pid, int pageNumber, int& frameNumber) {
    currentTime++;
    for (auto& entry : tlbEntries) {
        if (entry.pid == pid && entry.pageNumber == pageNumber) {
            entry.lastUsed = currentTime;
            frameNumber = entry.frameNumber;
            hits++;
            return true;
        }
    }

    misses++;
    return false;
}

void TLB::insert(int pid, int pageNumber, int frameNumber) {
    currentTime++;
    if (tlbEntries.size() < maxSize) {
        tlbEntries.push_back({pid, pageNumber, frameNumber, currentTime});
    } else {
        int lruIndex = 0;
        int minTime = std::numeric_limits<int>::max();

        for (int i = 0; i < tlbEntries.size(); ++i) {
            if (tlbEntries[i].lastUsed < minTime) {
                minTime = tlbEntries[i].lastUsed;
                lruIndex = i;
            }
        }
        
        tlbEntries[lruIndex] = {pid, pageNumber, frameNumber, currentTime};
    }
}

void TLB::printStats() const {
    std::cout << "--- TLB Stats ---" << std::endl;
    int totalAccesses = hits + misses;
    if (totalAccesses > 0) {
        double hitRate = static_cast<double>(hits) / totalAccesses * 100.0;
        std::cout << "Total Accesses: " << totalAccesses << std::endl;
        std::cout << "Hits:           " << hits << std::endl;
        std::cout << "Misses:         " << misses << std::endl;
        std::cout << "Hit Rate:       " << hitRate << "%" << std::endl;
    } else {
        std::cout << "No accesses to TLB." << std::endl;
    }
}