#ifndef DA_PROJECT_LIVE_RANGE_H
#define DA_PROJECT_LIVE_RANGE_H

#include <set>
#include <vector>
#include <iostream>

/**
 * @brief Representa o intervalo de vida (Live Range) de uma variável no programa.
 */
struct LiveRange {
    std::set<int> lines;
    std::set<int> defLines;
    std::set<int> lastUseLines;
    int startLine, endLine;
    bool isDefinition, isLastUse;
    
    /**
     * @brief Construtor a partir do conjunto de linhas.
     * @param lines_ Conjunto de linhas que compõem a live range.
     * @param defLines_ Conjunto de linhas onde a variável é definida.
     * @param lastUseLines_ Conjunto de linhas da última utilização da variável.
     * @param isDef Indica se a live range começa numa definição.
     * @param isLast Indica se a live range termina na última utilização.
     * \complexity O(n log n) no pior caso ao construir os conjuntos em memória.
     */
    LiveRange(
        const std::set<int>& lines_,
        const std::set<int>& defLines_ = {},
        const std::set<int>& lastUseLines_ = {},
        bool isDef = false,
        bool isLast = false
    ) : lines(lines_),
        defLines(defLines_),
        lastUseLines(lastUseLines_),
        isDefinition(isDef),
        isLastUse(isLast) {
        if (!lines.empty()) {
            startLine = *lines.begin();
            endLine = *lines.rbegin();
        } else {
            startLine = endLine = -1;
        }
    }
    
    /**
     * @brief Verifica se dois live ranges se sobrepõem.
     * @param other Outro live range.
     * @return True se partilharem pelo menos uma linha.
     * \complexity O(m log n) onde m é o número de linhas deste range e n é o do outro.
     */
    bool overlaps(const LiveRange& other) const {
        for (int line : lines)
            if (other.lines.count(line) > 0) return true;
        return false;
    }
};

#endif // DA_PROJECT_LIVE_RANGE_H