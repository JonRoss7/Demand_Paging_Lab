#ifndef CLOCK_H
#define CLOCK_H

#include <vector>

struct FrameEntry {
public:
    int pid;
    int pageNumber;
    bool referenced;
    bool modified;

    FrameEntry() : pid(-1), pageNumber(-1), referenced(false), modified(false) {}
    FrameEntry(int p, int pn, bool r, bool m) : pid(p), pageNumber(pn), referenced(r), modified(m) {}
};

class Clock {
private:
    std::vector<FrameEntry> frames;
    size_t clockHand;

public:
    Clock(int frameCount);
    int evictAndReplace(int pid, int pageNum, bool isModified, int& evictedPage, bool& isEvictedPageModified);
    void setReferenced(int frameIndex);
    void setModified(int frameIndex, bool isModified);
};

#endif