#ifndef DA_PROJECT_ALGORITHM_CONFIG_H
#define DA_PROJECT_ALGORITHM_CONFIG_H

#include <string>

/**
 * @struct AlgorithmConfig
 * @brief Configuracao do algoritmo de alocacao.
 */
struct AlgorithmConfig {
    enum AlgorithmType { BASIC, SPILLING, SPLITTING, FREE };
    
    AlgorithmType type;
    int maxRegisters;
    int parameter;
    
    /**
     * @brief Construtor da configuracao.
     * @param t Tipo do algoritmo.
     * @param regs Numero de registos disponiveis.
     * @param param Parametro extra (spills/splits).
     */
    AlgorithmConfig(AlgorithmType t = BASIC, int regs = 3, int param = 0)
        : type(t), maxRegisters(regs), parameter(param) {}
    
    /**
     * @brief Converte enum para string.
     * @param t Tipo do algoritmo.
     * @return String correspondente.
     * @complexity O(1).
     */
    static std::string typeToString(AlgorithmType t) {
        switch (t) {
            case BASIC: return "basic";
            case SPILLING: return "spilling";
            case SPLITTING: return "splitting";
            case FREE: return "free";
            default: return "unknown";
        }
    }
    
    /**
     * @brief Converte string para enum.
     * @param str String do tipo.
     * @return Enum correspondente (BASIC por defeito).
     * @complexity O(1).
     */
    static AlgorithmType stringToType(const std::string& str) {
        if (str == "basic") return BASIC;
        if (str == "spilling") return SPILLING;
        if (str == "splitting") return SPLITTING;
        if (str == "free") return FREE;
        return BASIC;
    }
    
    /**
     * @brief Serializa configuracao para debug/output.
     * @return String formatada.
     * @complexity O(1).
     */
    std::string toString() const {
        std::string result = "Algorithm: " + typeToString(type);
        if (type == SPILLING)
            result += " (max spills: " + std::to_string(parameter) + ")";
        else if (type == SPLITTING)
            result += " (max splits: " + std::to_string(parameter) + ")";
        result += ", registers: " + std::to_string(maxRegisters);
        return result;
    }
    
    /**
     * @brief Valida a configuracao.
     * @return True se valida.
     * @complexity O(1).
     */
    bool isValid() const {
        if (maxRegisters <= 0) return false;
        if (type == SPILLING || type == SPLITTING)
            if (parameter < 0) return false;
        return true;
    }
};

#endif
