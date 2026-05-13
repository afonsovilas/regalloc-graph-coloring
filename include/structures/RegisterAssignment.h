#ifndef DA_PROJECT_REGISTER_ASSIGNMENT_H
#define DA_PROJECT_REGISTER_ASSIGNMENT_H

#include <string>

/**
 * @struct RegisterAssignment
 * @brief Associacao de um web a um registo ou memoria.
 */
struct RegisterAssignment {
    int webId;
    std::string varName;
    int registerNum;
    bool isMemory;
    
    /**
     * @brief Construtor para registo valido.
     * @param id ID do web.
     * @param var Nome da variavel.
     * @param regNum Numero do registo.
     */
    RegisterAssignment(int id, const std::string& var, int regNum)
        : webId(id), varName(var), registerNum(regNum), isMemory(regNum < 0) {}
    
    /**
     * @brief Construtor para spill em memoria.
     * @param id ID do web.
     * @param var Nome da variavel.
     */
    RegisterAssignment(int id, const std::string& var)
        : webId(id), varName(var), registerNum(-1), isMemory(true) {}
    
    /**
     * @brief Serializa a atribuicao no formato de output.
     * @return String no formato "rX: webY" ou "M: webY".
     * @complexity O(1).
     */
    std::string toString() const {
        std::string prefix = isMemory ? "M" : ("r" + std::to_string(registerNum));
        return prefix + ": web" + std::to_string(webId);
    }
};

#endif
