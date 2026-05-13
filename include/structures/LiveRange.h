#ifndef DA_PROJECT_LIVE_RANGE_H
#define DA_PROJECT_LIVE_RANGE_H

#include <vector>
#include <set>
#include <iostream>

struct LiveRange {
    std::set<int> lines;
    int startLine, endLine;
    bool isDefinition, isLastUse;
    
    LiveRange(const std::set<int>& lines_, bool isDef = false, bool isLast = false)
        : lines(lines_), isDefinition(isDef), isLastUse(isLast) {
        if (!lines.empty()) {
            startLine = *lines.begin();
            endLine = *lines.rbegin();
        } else {
            startLine = endLine = -1;
        }
    }
    
    bool overlaps(const LiveRange& other) const {
        for (int line : lines)
            if (other.lines.count(line) > 0) return true;
        return false;
    }
};

#endif
