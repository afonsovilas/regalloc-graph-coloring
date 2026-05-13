#ifndef DA_PROJECT_WEB_H
#define DA_PROJECT_WEB_H

#include <vector>
#include <set>
#include <string>
#include "LiveRange.h"

/**
 * @struct Web
 * @brief Uniao de live ranges sobrepostos da mesma variavel.
 */
struct Web {
    int webId;
    std::string varName;
    std::set<int> lines;
    std::vector<LiveRange> liveRanges;
    
    /**
     * @brief Construtor de web.
     * @param id Identificador do web.
     * @param var Nome da variavel.
     */
    Web(int id, const std::string& var) : webId(id), varName(var) {}
    
    /**
     * @brief Adiciona um live range e atualiza a uniao de linhas.
     * @param lr Live range a adicionar.
     * @complexity O(m log n) onde m = linhas do live range e n = linhas do web.
     */
    void addLiveRange(const LiveRange& lr) {
        liveRanges.push_back(lr);
        for (int line : lr.lines)
            lines.insert(line);
    }
    
    /**
     * @brief Verifica se o web contem uma linha.
     * @param line Linha a procurar.
     * @return True se a linha existe no web.
     * @complexity O(log n) no set.
     */
    bool containsLine(int line) const {
        return lines.count(line) > 0;
    }
    
    /**
     * @brief Verifica sobreposicao entre dois webs.
     * @param other Outro web.
     * @return True se partilham pelo menos uma linha.
     * @complexity O(m log n) onde m = linhas deste web.
     */
    bool overlaps(const Web& other) const {
        for (int line : lines)
            if (other.lines.count(line) > 0) return true;
        return false;
    }
    
    /**
     * @brief Numero de linhas no web.
     * @return Tamanho do set.
     * @complexity O(1).
     */
    int getSize() const { return lines.size(); }
    
    /**
     * @brief Serializa o web para o formato de output.
     * @return String no formato "webX: ...".
     * @complexity O(n) onde n = numero de linhas.
     */
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
