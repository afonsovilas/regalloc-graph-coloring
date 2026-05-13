#ifndef DA_PROJECT_INPUT_PARSER_H
#define DA_PROJECT_INPUT_PARSER_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "../structures/LiveRange.h"
#include "../structures/Web.h"
#include "../structures/AlgorithmConfig.h"

/**
 * @class InputParser
 * @brief Parser para ficheiros de input do projeto de register allocation.
 * 
 * Responsavel por:
 * - Parse ranges.txt: variaveis com live ranges
 * - Parse registers.txt: numero de registos + algoritmo
 */
class InputParser {
private:
    /// Mapa: variável -> lista de live ranges
    std::map<std::string, std::vector<LiveRange>> variableLiveRanges;
    
    /// Configuração do algoritmo
    AlgorithmConfig algorithmConfig;
    
    /// Número de registos disponíveis
    int numRegisters;

public:
    InputParser() : numRegisters(0), 
                    algorithmConfig(AlgorithmConfig::BASIC, 0, 0) {}

    /**
     * @brief Parse ficheiro ranges.txt
     * 
     * Formato esperado:
     * ```
     * # comentário (ignorado)
     * sum: 7+,8,9,10-
     * i: 1+,2,3,4,5,6-
     * i: 9+,10,11,12-
     * ```
     * 
     * Onde:
     * - "+" = definição (assignment LHS)
     * - "-" = último uso (RHS)
     * - Múltiplas linhas da mesma variável = múltiplos live ranges
     * 
     * @param filename Caminho ficheiro ranges.txt
    * @return true se parse bem-sucedido, false caso contrario
    * @complexity O(L + T) onde L = linhas do ficheiro e T = tokens totais.
     */
    bool parseRanges(const std::string& filename) {
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "ERROR: Cannot open file: " << filename << std::endl;
            return false;
        }

        std::string line;
        int lineNumber = 0;

        while (std::getline(file, line)) {
            lineNumber++;
            
            // Ignorar linhas vazias e comentários
            if (line.empty() || line[0] == '#') continue;

            // Parse: "variableName: line1+,line2,line3-,..."
            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                std::cerr << "WARNING: Invalid format at line " << lineNumber << ": " << line << std::endl;
                continue;
            }

            std::string varName = line.substr(0, colonPos);
            std::string lineSpec = line.substr(colonPos + 1);

            // Trim whitespace
            varName = trim(varName);
            lineSpec = trim(lineSpec);

            // Parse os números de linha
            std::set<int> liveLines;
            bool isDefinition = false;
            bool isLastUse = false;

            // Split por vírgula
            std::stringstream ss(lineSpec);
            std::string token;
            while (std::getline(ss, token, ',')) {
                token = trim(token);
                if (token.empty()) continue;

                // Verificar se tem + ou - no final
                if (token.back() == '+') {
                    isDefinition = true;
                    token.pop_back();
                } else if (token.back() == '-') {
                    isLastUse = true;
                    token.pop_back();
                }

                try {
                    int lineNum = std::stoi(token);
                    liveLines.insert(lineNum);
                } catch (std::invalid_argument&) {
                    std::cerr << "WARNING: Invalid line number '" << token << "' at line " << lineNumber << std::endl;
                }
            }

            // Se conseguiu parsear pelo menos uma linha, adicionar
            if (!liveLines.empty()) {
                LiveRange lr(liveLines, isDefinition, isLastUse);
                variableLiveRanges[varName].push_back(lr);
            }
        }

        file.close();
        return true;
    }

    /**
     * @brief Parse ficheiro registers.txt
     * 
     * Formato esperado:
     * ```
     * # comentário
     * registers: 3
     * algorithm: basic
     * ```
     * 
     * Ou com parâmetro:
     * ```
     * registers: 2
     * algorithm: spilling, 2
     * ```
     * 
     * Algoritmos suportados:
     * - basic: greedy coloring simples
     * - spilling K: permite spill até K webs
     * - splitting K: permite split até K webs
     * - free: algoritmo livre
     * 
     * @param filename Caminho ficheiro registers.txt
    * @return true se parse bem-sucedido, false caso contrario
    * @complexity O(L) onde L = linhas do ficheiro.
     */
    bool parseRegisters(const std::string& filename) {
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "ERROR: Cannot open file: " << filename << std::endl;
            return false;
        }

        std::string line;
        int lineNumber = 0;

        while (std::getline(file, line)) {
            lineNumber++;
            
            if (line.empty() || line[0] == '#') continue;

            // Parse "registers: N"
            if (line.find("registers:") != std::string::npos) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string numStr = line.substr(colonPos + 1);
                    numStr = trim(numStr);
                    try {
                        numRegisters = std::stoi(numStr);
                        if (numRegisters <= 0) {
                            std::cerr << "ERROR: Number of registers must be positive at line " << lineNumber << std::endl;
                            return false;
                        }
                    } catch (std::invalid_argument&) {
                        std::cerr << "ERROR: Invalid number of registers at line " << lineNumber << std::endl;
                        return false;
                    }
                }
            }

            // Parse "algorithm: TYPE" ou "algorithm: TYPE, PARAM"
            if (line.find("algorithm:") != std::string::npos) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string algoSpec = line.substr(colonPos + 1);
                    algoSpec = trim(algoSpec);

                    // Check for comma (parameter)
                    size_t commaPos = algoSpec.find(',');
                    std::string algoType = algoSpec;
                    int param = 0;

                    if (commaPos != std::string::npos) {
                        algoType = algoSpec.substr(0, commaPos);
                        std::string paramStr = algoSpec.substr(commaPos + 1);
                        algoType = trim(algoType);
                        paramStr = trim(paramStr);
                        try {
                            param = std::stoi(paramStr);
                        } catch (std::invalid_argument&) {
                            std::cerr << "WARNING: Invalid algorithm parameter at line " << lineNumber << std::endl;
                        }
                    }

                    AlgorithmConfig::AlgorithmType type = AlgorithmConfig::stringToType(algoType);
                    algorithmConfig = AlgorithmConfig(type, numRegisters, param);
                }
            }
        }

        file.close();

        // Validar
        if (!algorithmConfig.isValid()) {
            std::cerr << "ERROR: Invalid algorithm configuration" << std::endl;
            return false;
        }

        return true;
    }

    /**
     * @brief Construir webs a partir dos live ranges parseados
     * 
     * Estratégia:
     * 1. Para cada variável com múltiplos live ranges
     * 2. Merge ranges que se sobrepõem (greedy approach)
     * 3. Retornar lista de webs únicos
     * 
    * @complexity O(v * r^2) onde v = variaveis e r = ranges por variavel.
     * 
     * @return Vector de webs construídos
     */
    std::vector<Web> buildWebs() const {
        std::vector<Web> webs;
        int webId = 0;

        for (const auto& [varName, ranges] : variableLiveRanges) {
            // Começar com primeiro range
            std::set<int> mergedLines;
            bool isMergedDef = false;
            bool isMergedLastUse = false;

            for (const auto& range : ranges) {
                // Se não sobrepõe com merged, criar novo web
                bool needsNewWeb = false;
                for (int line : range.lines) {
                    if (mergedLines.find(line) == mergedLines.end()) {
                        // Se já temos algo em mergedLines, talvez seja novo web
                        if (!mergedLines.empty() && range.lines.size() > 0) {
                            // Verificar se primeiro/último da linha é vizinho
                            int minMerged = *mergedLines.begin();
                            int maxMerged = *mergedLines.rbegin();
                            int minRange = *range.lines.begin();
                            int maxRange = *range.lines.rbegin();
                            
                            // Se há gap > 0, pode ser novo web ou continuação
                            if (minRange > maxMerged + 1 && minMerged > maxRange + 1) {
                                needsNewWeb = true;
                            }
                        }
                    }
                }

                if (needsNewWeb && !mergedLines.empty()) {
                    // Salvar web anterior
                    Web web(webId++, varName);
                    for (int line : mergedLines) {
                        web.lines.insert(line);
                    }
                    webs.push_back(web);

                    // Começar novo
                    mergedLines = range.lines;
                    isMergedDef = range.isDefinition;
                    isMergedLastUse = range.isLastUse;
                } else {
                    // Merge: adicionar linhas do range ao merged
                    for (int line : range.lines) {
                        mergedLines.insert(line);
                    }
                    if (range.isDefinition) isMergedDef = true;
                    if (range.isLastUse) isMergedLastUse = true;
                }
            }

            // Salvar último web
            if (!mergedLines.empty()) {
                Web web(webId++, varName);
                for (int line : mergedLines) {
                    web.lines.insert(line);
                }
                webs.push_back(web);
            }
        }

        return webs;
    }

    /// @brief Acesso aos live ranges por variavel.
    const std::map<std::string, std::vector<LiveRange>>& getVariableLiveRanges() const {
        return variableLiveRanges;
    }
    
    /// @brief Acesso a configuracao do algoritmo.
    const AlgorithmConfig& getAlgorithmConfig() const {
        return algorithmConfig;
    }
    
    /// @brief Numero de registos disponiveis.
    int getNumRegisters() const {
        return numRegisters;
    }

private:
    /**
     * @brief Remove espacos no inicio e fim.
     * @param str String a processar.
     * @return String sem espacos.
     * @complexity O(n) onde n = tamanho da string.
     */
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
};

#endif
