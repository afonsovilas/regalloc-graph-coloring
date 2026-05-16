#ifndef DA_PROJECT_GREEDY_COLORING_H
#define DA_PROJECT_GREEDY_COLORING_H

#include <map>
#include <set>
#include <vector>
#include "../structures/InterferenceGraph.h"

/**
 * @struct ColoringResult
 * @brief Resultado do greedy graph coloring.
 */
struct ColoringResult {
    std::map<int,int> colors;
    std::set<int> spilled;
};

/**
 * @class GreedyColoring
 * @brief Greedy graph-coloring (Figura 9 do enunciado).
 */
class GreedyColoring {
public:
    /**
     * @brief Executa o algoritmo de Figura 9 com N cores.
     * @param graph Grafo de interferencia.
     * @param N Numero de cores disponiveis.
     * @return Coloring assignment + conjunto de webs spilled.
     * @complexity O(V * (V + E)) no pior caso.
     */
    static ColoringResult run(const InterferenceGraph& graph, int N) {
        ColoringResult result;
        if (N <= 0) {
            for (const auto& pair : graph.getNodes())
                result.spilled.insert(pair.first);
            return result;
        }

        std::map<int, std::set<int>> G;
        for (const auto& pair : graph.getNodes())
            G[pair.first] = pair.second.interferesWith;

        std::vector<int> S;

        // Phase 1: Clearing cliques
        while (!G.empty()) {
            std::vector<int> lowDegree;
            for (auto& pair : G)
                if (pair.second.size() < N)
                    lowDegree.push_back(pair.first);

            for (int n : lowDegree) {
                for (int neighbor : G[n]) {
                    auto it = G.find(neighbor);
                    if (it != G.end()) it->second.erase(n);
                }
                G.erase(n);
                S.push_back(n);
            }

            if (lowDegree.empty()) {
                int K = G.begin()->first;
                int bestDeg = G.begin()->second.size();
                for (auto& pair : G)
                    if (pair.second.size() > bestDeg) {
                        bestDeg = pair.second.size();
                        K = pair.first;
                    }

                for (int neighbor : G[K]) {
                    auto it = G.find(neighbor);
                    if (it != G.end()) it->second.erase(K);
                }
                G.erase(K);
                result.spilled.insert(K);
            }
        }

        // Phase 2: Coloring
        while (!S.empty()) {
            int n = S.back();
            S.pop_back();

            std::set<int> used;
            for (int neighbor : graph.getNodes().at(n).interferesWith) {
                if (result.spilled.count(neighbor)) continue;
                auto cit = result.colors.find(neighbor);
                if (cit != result.colors.end()) used.insert(cit->second);
            }
            for (int c = 0; c < N; c++)
                if (!used.count(c)) { result.colors[n] = c; break; }
        }

        return result;
    }
};

#endif
