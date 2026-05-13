#ifndef DA_PROJECT_WEB_H
#define DA_PROJECT_WEB_H

#include <vector>
#include <set>
#include <string>
#include "LiveRange.h"

struct Web {
    int webId;
    std::string varName;
    std::set<int> lines;
    std::vector<LiveRange> liveRanges;
    
    Web(int id, const std::string& var) : webId(id), varName(var) {}
    
    void addLiveRange(const LiveRange& lr) {
        liveRanges.push_back(lr);
        for (int line : lr.lines)
            lines.insert(line);
    }
    
    bool containsLine(int line) const {
        return lines.count(line) > 0;
    }
    
    bool overlaps(const Web& other) const {
        for (int line : lines)
            if (other.lines.count(line) > 0) return true;
        return false;
    }
    
    int getSize() const { return lines.size(); }
    
    std::string toString() const {
        std::string result = "web" + std::to_string(webId) + ": ";
        bool first = true;
        for (int line : lines) {
            if (!first) result += ",";
            result += std::to_string(line);
            first = false;
        }
        return result;
    }
};

#endif
