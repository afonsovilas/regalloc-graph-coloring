#ifndef DA_PROJECT_REGISTER_ASSIGNMENT_H
#define DA_PROJECT_REGISTER_ASSIGNMENT_H

#include <string>

struct RegisterAssignment {
    int webId;
    std::string varName;
    int registerNum;
    bool isMemory;
    
    RegisterAssignment(int id, const std::string& var, int regNum)
        : webId(id), varName(var), registerNum(regNum), isMemory(regNum < 0) {}
    
    RegisterAssignment(int id, const std::string& var)
        : webId(id), varName(var), registerNum(-1), isMemory(true) {}
    
    std::string toString() const {
        std::string prefix = isMemory ? "M" : ("r" + std::to_string(registerNum));
        return prefix + ": web" + std::to_string(webId);
    }
};

#endif
