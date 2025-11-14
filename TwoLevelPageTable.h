#ifndef TWOLEVELPAGETABLE_H
#define TWOLEVELPAGETABLE_H

#include <map>
#include <set>
#include <utility>

class TwoLevelPageTable {
private:
    std::map<std::pair<int, int>, int> firstLevel; // (pid, outer_page) -> second-level table
    std::map<std::pair<int, int>, int> secondLevel; // (pid, inner_page) -> frame#
    std::set<std::pair<int, int>> memoryPages;

public:
    TwoLevelPageTable(int pageCount);
    bool isInMemory(int pid, int pageNum) const;
    int getFrame(int pid, int pageNum) const;
    void loadPage(int pid, int pageNum, int frameNum);
    void printStats() const;
};

#endif