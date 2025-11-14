#include "TwoLevelPageTable.h"
#include <iostream>

const int INNER_BITS = 10;
const int INNER_MASK = (1 << INNER_BITS) - 1;


static int nextTableID = 1;

TwoLevelPageTable::TwoLevelPageTable(int pageCount) {
}

bool TwoLevelPageTable::isInMemory(int pid, int pageNum) const {
    return memoryPages.find({pid, pageNum}) != memoryPages.end();
}

int TwoLevelPageTable::getFrame(int pid, int pageNum) const {
    int outerPage = pageNum >> INNER_BITS;
    int innerPage = pageNum & INNER_MASK;


    if (firstLevel.find({pid, outerPage}) == firstLevel.end()) {
        return -1; 
    }
    int tableID = firstLevel.at({pid, outerPage});

    
    if (secondLevel.find({tableID, innerPage}) == secondLevel.end()) {
        return -1; 
    }

    return secondLevel.at({tableID, innerPage});
}

void TwoLevelPageTable::loadPage(int pid, int pageNum, int frameNum) {
    int outerPage = pageNum >> INNER_BITS;
    int innerPage = pageNum & INNER_MASK;

 
    int tableID;
    if (firstLevel.find({pid, outerPage}) == firstLevel.end()) {
        tableID = nextTableID++;
        firstLevel[{pid, outerPage}] = tableID;
    } else {
        tableID = firstLevel[{pid, outerPage}];
    }


    secondLevel[{tableID, innerPage}] = frameNum;

    memoryPages.insert({pid, pageNum});
}

void TwoLevelPageTable::printStats() const {
    std::cout << "Memory Pages Tracked: " << memoryPages.size() << std::endl;
    std::cout << "First Level Entries: " << firstLevel.size() << std::endl;
    std::cout << "Second Level Entries: " << secondLevel.size() << std::endl;
}