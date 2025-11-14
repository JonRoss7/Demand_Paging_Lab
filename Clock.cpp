#include "Clock.h"
#include <iostream>

Clock::Clock(int frameCount) : clockHand(0) {
    frames.resize(frameCount, FrameEntry());
}

int Clock::evictAndReplace(int pid, int pageNum, bool isModified, int& evictedPage, bool& isEvictedPageModified) {
    
    for (int i = 0; i < frames.size(); ++i) {
        if (frames[i].pid == -1) {
            frames[i] = {pid, pageNum, true, isModified};
            evictedPage = -1;
            isEvictedPageModified = false;
            return i;
        }
    }

    while (true) {
        for (int i = 0; i < frames.size(); ++i) {
            FrameEntry& entry = frames[clockHand];

            if (!entry.referenced && !entry.modified) {
                evictedPage = entry.pageNumber;
                isEvictedPageModified = entry.modified;
                entry = {pid, pageNum, true, isModified};
                int frameIndex = clockHand;
                clockHand = (clockHand + 1) % frames.size();
                return frameIndex;
            }
            
            clockHand = (clockHand + 1) % frames.size();
        }

        for (int i = 0; i < frames.size(); ++i) {
            FrameEntry& entry = frames[clockHand];

            if (!entry.referenced && entry.modified) {
                evictedPage = entry.pageNumber;
                isEvictedPageModified = entry.modified;
                entry = {pid, pageNum, true, isModified};
                int frameIndex = clockHand;
                clockHand = (clockHand + 1) % frames.size();
                return frameIndex;
            }
            
            entry.referenced = false;
            clockHand = (clockHand + 1) % frames.size();
        }
    }
}

void Clock::setReferenced(int frameIndex) {
    if (frameIndex >= 0 && frameIndex < frames.size()) {
        frames[frameIndex].referenced = true;
    }
}

void Clock::setModified(int frameIndex, bool isModified) {
    if (frameIndex >= 0 && frameIndex < frames.size()) {
        if (isModified) {
            frames[frameIndex].modified = true;
        }
        frames[frameIndex].referenced = true;
    }
}