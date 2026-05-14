#ifndef DA_PROJECT_GRAPH_COLORING_H
#define DA_PROJECT_GRAPH_COLORING_H

#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "../structures/InterferenceGraph.h"
#include "../structures/RegisterAssignment.h"
#include "../structures/Web.h"

/**
 * @struct ColoringResult
 * @brief Resultado do graph coloring basico.
 */
struct ColoringResult {
    bool success;
    int registersUsed;
    std::map<int, int> colors;
    std::vector<RegisterAssignment> assignments;
};

/**
 * @brief Executa o graph coloring basico (T2.1).
 *
 * Fase 1: simplificacao remove nos com grau < K e empilha.
 * Fase 2: desempilha e atribui a menor cor disponivel.
 *
 * @param graph Grafo de interferencia.
 * @param webs Lista de webs para obter nomes e ids.
 * @param maxRegisters Numero maximo de registos (K).
 * @return Resultado com cores e atribuicoes.
 * @complexity O(w^2 * c) na simplificacao (w = webs, c = custo de overlaps) e
 *             O(w * d) no coloring (d = grau medio).
 */
static ColoringResult colorGraphBasic(
    const InterferenceGraph& graph,
    const std::vector<Web>& webs,
    int maxRegisters
) {
    ColoringResult result{};
    result.success = false;
    result.registersUsed = 0;

    if (maxRegisters <= 0) {
        return result;
    }

    std::map<int, std::string> webNames;
    for (const auto& web : webs) {
        webNames[web.webId] = web.varName;
    }

    InterferenceGraph temp = graph;
    std::stack<int> stack;

    while (temp.getNumNodes() > 0) {
        int candidate = -1;
        for (const auto& pair : temp.getNodes()) {
            if (pair.second.getDegree() < maxRegisters) {
                candidate = pair.first;
                break;
            }
        }

        if (candidate == -1) {
            return result;
        }

        stack.push(candidate);
        temp.removeNode(candidate);
    }

    std::map<int, int> colors;
    int maxColor = -1;

    while (!stack.empty()) {
        int node = stack.top();
        stack.pop();

        std::set<int> usedColors;
        auto it = graph.getNodes().find(node);
        if (it != graph.getNodes().end()) {
            for (int neighbor : it->second.interferesWith) {
                auto colorIt = colors.find(neighbor);
                if (colorIt != colors.end()) {
                    usedColors.insert(colorIt->second);
                }
            }
        }

        int chosen = -1;
        for (int c = 0; c < maxRegisters; c++) {
            if (usedColors.find(c) == usedColors.end()) {
                chosen = c;
                break;
            }
        }

        if (chosen == -1) {
            return result;
        }

        colors[node] = chosen;
        if (chosen > maxColor) maxColor = chosen;
    }

    result.success = true;
    result.colors = colors;
    result.registersUsed = maxColor + 1;

    result.assignments.reserve(webs.size());
    for (const auto& web : webs) {
        auto colorIt = colors.find(web.webId);
        if (colorIt != colors.end()) {
            result.assignments.emplace_back(web.webId, web.varName, colorIt->second);
        } else {
            result.assignments.emplace_back(web.webId, web.varName);
        }
    }

    return result;
}

#endif
