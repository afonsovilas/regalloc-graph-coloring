#ifndef DA_PROJECT_ALGORITHM_CONFIG_H
#define DA_PROJECT_ALGORITHM_CONFIG_H

#include <string>

struct AlgorithmConfig {
    enum AlgorithmType { BASIC, SPILLING, SPLITTING, FREE };
    
    AlgorithmType type;
    int maxRegisters;
    int parameter;
    
    AlgorithmConfig(AlgorithmType t = BASIC, int regs = 3, int param = 0)
        : type(t), maxRegisters(regs), parameter(param) {}
    
    static std::string typeToString(AlgorithmType t) {
        switch (t) {
            case BASIC: return "basic";
            case SPILLING: return "spilling";
            case SPLITTING: return "splitting";
            case FREE: return "free";
            default: return "unknown";
        }
    }
    
    static AlgorithmType stringToType(const std::string& str) {
        if (str == "basic") return BASIC;
        if (str == "spilling") return SPILLING;
        if (str == "splitting") return SPLITTING;
        if (str == "free") return FREE;
        return BASIC;
    }
    
    std::string toString() const {
        std::string result = "Algorithm: " + typeToString(type);
        if (type == SPILLING)
            result += " (max spills: " + std::to_string(parameter) + ")";
        else if (type == SPLITTING)
            result += " (max splits: " + std::to_string(parameter) + ")";
        result += ", registers: " + std::to_string(maxRegisters);
        return result;
    }
    
    bool isValid() const {
        if (maxRegisters <= 0) return false;
        if (type == SPILLING || type == SPLITTING)
            if (parameter < 0) return false;
        return true;
    }
};

#endif
