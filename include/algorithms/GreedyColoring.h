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
                if (static_cast<int>(pair.second.size()) < N)
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
                    if (static_cast<int>(pair.second.size()) > bestDeg) {
                        bestDeg = static_cast<int>(pair.second.size());
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

    /**
     * @brief Coloracao sem spilling. Falha se nao for colorivel com N cores.
     * @param graph Grafo de interferencia.
     * @param N Numero de cores disponiveis.
     * @param colorsOut Cores atribuida aos nos.
     * @return True se colorivel sem spill.
     * @complexity O(V * (V + E)) no pior caso.
     */
    static bool colorWithoutSpill(
        const InterferenceGraph& graph,
        int N,
        std::map<int, int>& colorsOut
    ) {
        colorsOut.clear();
        if (N <= 0) return false;

        std::map<int, std::set<int>> G;
        for (const auto& pair : graph.getNodes())
            G[pair.first] = pair.second.interferesWith;

        std::vector<int> S;

        while (!G.empty()) {
            int candidate = -1;
            for (const auto& pair : G) {
                if (static_cast<int>(pair.second.size()) < N) {
                    candidate = pair.first;
                    break;
                }
            }

            if (candidate == -1) return false;

            for (int neighbor : G[candidate]) {
                auto it = G.find(neighbor);
                if (it != G.end()) it->second.erase(candidate);
            }
            G.erase(candidate);
            S.push_back(candidate);
        }

        while (!S.empty()) {
            int n = S.back();
            S.pop_back();

            std::set<int> used;
            for (int neighbor : graph.getNodes().at(n).interferesWith) {
                auto cit = colorsOut.find(neighbor);
                if (cit != colorsOut.end()) used.insert(cit->second);
            }

            int chosen = -1;
            for (int c = 0; c < N; c++)
                if (!used.count(c)) { chosen = c; break; }

            if (chosen == -1) return false;
            colorsOut[n] = chosen;
        }

        return true;
    }

    /**
     * @brief Coloracao DSATUR com N cores. Nos sem cor ficam como spilled.
     * @param graph Grafo de interferencia.
     * @param N Numero de cores disponiveis.
     * @return ColoringResult com cores e spills.
     * @complexity O(V^2 + E) no pior caso.
     */
    static ColoringResult runDsatur(const InterferenceGraph& graph, int N) {
        ColoringResult result;
        if (N <= 0) {
            for (const auto& pair : graph.getNodes())
                result.spilled.insert(pair.first);
            return result;
        }

        std::set<int> uncolored;
        for (const auto& pair : graph.getNodes())
            uncolored.insert(pair.first);

        std::map<int, std::set<int>> neighborColors;

        while (!uncolored.empty()) {
            int pick = -1;
            int bestSat = -1;
            int bestDeg = -1;

            for (int node : uncolored) {
                int sat = static_cast<int>(neighborColors[node].size());
                int deg = graph.getNodes().at(node).getDegree();
                if (sat > bestSat || (sat == bestSat && deg > bestDeg)) {
                    bestSat = sat;
                    bestDeg = deg;
                    pick = node;
                }
            }

            std::set<int> used;
            for (int neighbor : graph.getNodes().at(pick).interferesWith) {
                auto it = result.colors.find(neighbor);
                if (it != result.colors.end()) used.insert(it->second);
            }

            int chosen = -1;
            for (int c = 0; c < N; c++)
                if (!used.count(c)) { chosen = c; break; }

            if (chosen == -1) {
                result.spilled.insert(pick);
            } else {
                result.colors[pick] = chosen;
                for (int neighbor : graph.getNodes().at(pick).interferesWith) {
                    neighborColors[neighbor].insert(chosen);
                }
            }

            uncolored.erase(pick);
        }

        return result;
    }
};

#endif
