#ifndef DA_PROJECT_LIVE_RANGE_H
#define DA_PROJECT_LIVE_RANGE_H

#include <vector>
#include <set>
#include <iostream>

/**
 * @struct LiveRange
 * @brief Intervalo contiguo onde uma variavel esta viva.
 */
struct LiveRange {
    std::set<int> lines;
    int startLine, endLine;
    bool isDefinition, isLastUse;
    
    /**
     * @brief Construtor a partir do conjunto de linhas.
     * @param lines_ Linhas onde a variavel esta viva.
     * @param isDef True se o intervalo comeca com definicao (+).
     * @param isLast True se termina com ultima utilizacao (-).
     * @complexity O(n log n) no pior caso ao construir o set.
     */
    LiveRange(const std::set<int>& lines_, bool isDef = false, bool isLast = false)
        : lines(lines_), isDefinition(isDef), isLastUse(isLast) {
        if (!lines.empty()) {
            startLine = *lines.begin();
            endLine = *lines.rbegin();
        } else {
            startLine = endLine = -1;
        }
    }
    
    /**
     * @brief Verifica se dois live ranges se sobrepoem.
     * @param other Outro live range.
     * @return True se partilham pelo menos uma linha.
     * @complexity O(m log n) onde m = linhas deste range e n = linhas do outro.
     */
    bool overlaps(const LiveRange& other) const {
        for (int line : lines)
            if (other.lines.count(line) > 0) return true;
        return false;
    }
};

#endif
