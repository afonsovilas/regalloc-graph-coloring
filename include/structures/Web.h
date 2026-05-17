#ifndef DA_PROJECT_WEB_H
#define DA_PROJECT_WEB_H

#include <vector>
#include <set>
#include <string>
#include "LiveRange.h"

/**
 * @struct Web
 * @brief União de live ranges sobrepostos da mesma variável.
 */
struct Web {
    int webId;
    std::string varName;
    std::set<int> lines;
    std::set<int> defLines;
    std::set<int> lastUseLines;
    std::vector<LiveRange> liveRanges;
    
    /**
     * @brief Construtor de web.
     * @param id Identificador do web.
     * @param var Nome da variável.
     */
    Web(int id, const std::string& var) : webId(id), varName(var) {}
    
    /**
     * @brief Adiciona um live range e atualiza a união de linhas.
     * @param lr Live range a adicionar.
     * \complexity O(m log n) onde m é o número de linhas do live range e n o de linhas da web.
     */
    void addLiveRange(const LiveRange& lr) {
        liveRanges.push_back(lr);
        for (int line : lr.lines)
            lines.insert(line);
        for (int line : lr.defLines)
            defLines.insert(line);
        for (int line : lr.lastUseLines)
            lastUseLines.insert(line);
    }

    /**
     * @brief Merge de outro web neste.
     * @param other Web a fundir.
     * \complexity O(m log n) onde m é o número de linhas do outro web.
     */
    void mergeFrom(const Web& other) {
        for (int line : other.lines)
            lines.insert(line);
        for (int line : other.defLines)
            defLines.insert(line);
        for (int line : other.lastUseLines)
            lastUseLines.insert(line);
        for (const auto& lr : other.liveRanges)
            liveRanges.push_back(lr);
    }
    
    /**
     * @brief Verifica sobreposição simples entre dois webs.
     * @param other Outro web.
     * @return True se partilharem pelo menos uma linha.
     * \complexity O(m log n) onde m é o número de linhas deste web.
     */
    bool overlaps(const Web& other) const {
        for (int line : lines)
            if (other.lines.count(line) > 0) return true;
        return false;
    }

    /**
     * @brief Verifica interferência estrutural considerando limites de definição/uso.
     * @param other Outro web.
     * @return True se interferem.
     * \complexity O(m log n) onde m é o número de linhas deste web.
     */
    bool interferesWith(const Web& other) const {
        for (int line : lines) {
            if (other.lines.count(line) == 0) continue;

            bool thisDef = defLines.count(line) > 0;
            bool thisLast = lastUseLines.count(line) > 0;
            bool otherDef = other.defLines.count(line) > 0;
            bool otherLast = other.lastUseLines.count(line) > 0;

            bool boundaryPair = (thisDef && otherLast) || (otherDef && thisLast);
            if (!boundaryPair) return true;
        }
        return false;
    }
    
    /**
     * @brief Número de linhas no web.
     * @return Tamanho do set.
     * \complexity O(1).
     */
    int getSize() const { return lines.size(); }
    
    /**
     * @brief Serializa o web para o formato de output.
     * @return String no formato "webX: ...".
     * \complexity O(n) onde n é o número de linhas.
     */
    std::string toString() const {
        std::string result = "web" + std::to_string(webId) + ": ";
        bool first = true;
        for (int line : lines) {
            if (!first) result += ",";
            result += std::to_string(line);
            if (defLines.count(line) > 0 && lastUseLines.count(line) == 0) {
                result += "+";
            } else if (defLines.count(line) == 0 && lastUseLines.count(line) > 0) {
                result += "-";
            } else if (defLines.count(line) > 0 && lastUseLines.count(line) > 0) {
                result += "+";
            }
            first = false;
        }
        return result;
    }
};

#endif