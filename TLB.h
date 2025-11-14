#ifndef TLB_H
#define TLB_H

#include <vector>

struct TLBEntry {
public:
    int pid;
    int pageNumber;
    int frameNumber;
    int lastUsed;
};

class TLB {
private:
    std::vector<TLBEntry> tlbEntries;
    int currentTime;
    int maxSize;

    // Private members to track stats
    int hits;
    int misses;

public:
    TLB(int size);
    bool lookup(int pid, int pageNumber, int& frameNumber);
    void insert(int pid, int pageNumber, int frameNumber);
    void printStats() const;
};

#endif // TLB_H