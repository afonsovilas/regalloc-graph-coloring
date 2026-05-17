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
#include "../structures/InterferenceGraph.h"

/**
 * @class InputParser
 * @brief Parser para ficheiros de input do projeto de register allocation.
 * * Responsável por:
 * - Parse ranges.txt: variáveis com live ranges
 * - Parse registers.txt: número de registos + algoritmo
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
    InputParser() : algorithmConfig(AlgorithmConfig::BASIC, 0, 0),
                    numRegisters(0) {}

    /**
     * @brief Parse do ficheiro ranges.txt.
     * * Formato esperado:
     * ```
     * # comentário (ignorado)
     * sum: 7+,8,9,10-
     * i: 1+,2,3,4,5,6-
     * i: 9+,10,11,12-
     * ```
     * * Onde:
     * - "+" = definição (assignment LHS)
     * - "-" = último uso (RHS)
     * - Múltiplas linhas da mesma variável = múltiplos live ranges
     * * @param filename Caminho do ficheiro ranges.txt.
     * @return true se o parse for bem-sucedido, false caso contrário.
     * \complexity O(L + T) onde L = linhas do ficheiro e T = tokens totais.
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
            std::set<int> defLines;
            std::set<int> lastUseLines;
            bool isDefinition = false;
            bool isLastUse = false;

            // Split por vírgula
            std::stringstream ss(lineSpec);
            std::string token;
            while (std::getline(ss, token, ',')) {
                token = trim(token);
                if (token.empty()) continue;

                // Verificar se tem + ou - no final
                bool tokenDef = false;
                bool tokenLast = false;
                if (token.back() == '+') {
                    tokenDef = true;
                    token.pop_back();
                } else if (token.back() == '-') {
                    tokenLast = true;
                    token.pop_back();
                }

                try {
                    int lineNum = std::stoi(token);
                    liveLines.insert(lineNum);
                    if (tokenDef) {
                        defLines.insert(lineNum);
                        isDefinition = true;
                    }
                    if (tokenLast) {
                        lastUseLines.insert(lineNum);
                        isLastUse = true;
                    }
                } catch (std::invalid_argument&) {
                    std::cerr << "WARNING: Invalid line number '" << token << "' at line " << lineNumber << std::endl;
                }
            }

            // Se conseguiu parsear pelo menos uma linha, adicionar
            if (!liveLines.empty()) {
                LiveRange lr(liveLines, defLines, lastUseLines, isDefinition, isLastUse);
                variableLiveRanges[varName].push_back(lr);
            }
        }

        file.close();
        return true;
    }

    /**
     * @brief Parse do ficheiro registers.txt.
     * * Formato esperado:
     * ```
     * # comentário
     * registers: 3
     * algorithm: basic
     * ```
     * * Ou com parâmetro:
     * ```
     * registers: 2
     * algorithm: spilling, 2
     * ```
     * * Algoritmos suportados:
     * - basic: greedy coloring simples
     * - spilling K: permite spill até K webs
     * - splitting K: permite split até K webs
     * - free: algoritmo livre
     * * @param filename Caminho do ficheiro registers.txt.
     * @return true se o parse for bem-sucedido, false caso contrário.
     * \complexity O(L) onde L = linhas do ficheiro.
     */
    bool parseRegisters(const std::string& filename) {
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "ERROR: Cannot open file: " << filename << std::endl;
            return false;
        }

        std::string line;
        int lineNumber = 0;
        bool hasRegisters = false;
        bool hasAlgorithm = false;
        int parsedRegisters = 0;
        AlgorithmConfig::AlgorithmType parsedType = AlgorithmConfig::BASIC;
        int parsedParam = 0;

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
                        parsedRegisters = std::stoi(numStr);
                        if (parsedRegisters <= 0) {
                            std::cerr << "ERROR: Number of registers must be positive at line " << lineNumber << std::endl;
                            return false;
                        }
                        hasRegisters = true;
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

                    parsedType = AlgorithmConfig::stringToType(algoType);
                    parsedParam = param;
                    hasAlgorithm = true;
                }
            }
        }

        file.close();

        if (hasRegisters) numRegisters = parsedRegisters;
        if (hasAlgorithm) algorithmConfig = AlgorithmConfig(parsedType, numRegisters, parsedParam);

        // Validar
        if (!algorithmConfig.isValid()) {
            std::cerr << "ERROR: Invalid algorithm configuration" << std::endl;
            return false;
        }

        return true;
    }

    /**
     * @brief Construir webs a partir dos live ranges parseados.
     * * Estratégia:
     * 1. Para cada variável com múltiplos live ranges.
     * 2. Merge de ranges que se sobrepõem (greedy approach).
     * 3. Retornar lista de webs únicos.
     * * @return Vetor de webs construídos.
     * \complexity O(V_vars * R^2) onde V_vars é o número de variáveis e R o número de sub-intervalos por variável.
     */
    std::vector<Web> buildWebs() const {
        std::vector<Web> webs;
        int webId = 0;

        for (const auto& [varName, ranges] : variableLiveRanges) {
            std::vector<Web> varWebs;

            for (const auto& range : ranges) {
                std::vector<size_t> overlaps;
                for (size_t i = 0; i < varWebs.size(); i++) {
                    bool intersects = false;
                    for (int line : range.lines) {
                        if (varWebs[i].lines.count(line) > 0) {
                            intersects = true;
                            break;
                        }
                    }
                    if (intersects) overlaps.push_back(i);
                }

                if (overlaps.empty()) {
                    Web web(webId++, varName);
                    web.addLiveRange(range);
                    varWebs.push_back(web);
                } else {
                    size_t target = overlaps.front();
                    varWebs[target].addLiveRange(range);

                    for (size_t k = 1; k < overlaps.size(); k++) {
                        size_t idx = overlaps[k];
                        varWebs[target].mergeFrom(varWebs[idx]);
                    }

                    for (size_t k = overlaps.size(); k-- > 1;) {
                        size_t idx = overlaps[k];
                        varWebs.erase(varWebs.begin() + idx);
                    }
                }
            }

            for (auto& web : varWebs)
                webs.push_back(web);
        }

        return webs;
    }

    /**
     * @brief Constrói o grafo de interferência a partir de webs.
     * @param webs Lista de webs.
     * @return Grafo de interferência construído.
     * \complexity O(W^2 * L) onde W é o número total de webs e L é a média de linhas de código por web.
     */
    InterferenceGraph buildInterferenceGraph(const std::vector<Web>& webs) const {
        InterferenceGraph graph;

        for (const auto& web : webs) {
            graph.addNode(web.webId);
        }

        for (size_t i = 0; i < webs.size(); i++) {
            for (size_t j = i + 1; j < webs.size(); j++) {
                if (webs[i].interferesWith(webs[j])) {
                    graph.addInterference(webs[i].webId, webs[j].webId);
                }
            }
        }

        return graph;
    }

    /// @brief Acesso aos live ranges por variável.
    const std::map<std::string, std::vector<LiveRange>>& getVariableLiveRanges() const {
        return variableLiveRanges;
    }
    
    /// @brief Acesso à configuração do algoritmo.
    const AlgorithmConfig& getAlgorithmConfig() const {
        return algorithmConfig;
    }
    
    /// @brief Número de registos disponíveis.
    int getNumRegisters() const {
        return numRegisters;
    }

private:
    /**
     * @brief Remove espaços no início e no fim de uma string.
     * @param str String a processar.
     * @return String limpa.
     * \complexity O(n) onde n é o tamanho da string.
     */
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
};

#endif // DA_PROJECT_INPUT_PARSER_H