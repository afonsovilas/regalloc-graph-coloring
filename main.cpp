/**
 * @file main.cpp
 * @brief Interface CLI e batch mode do projeto.
 */

#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <algorithm>
#include "include/parsers/InputParser.h"
#include "include/structures/RegisterAssignment.h"
#include "include/algorithms/GreedyColoring.h"

/**
 * @struct AllocationResult
 * @brief Resultado da alocacao para output.
 */
struct AllocationResult {
    int registerCount;
    std::vector<RegisterAssignment> assignments;
    bool successful;
    std::vector<Web> webs;
    std::vector<int> spilledWebs;
    std::vector<std::pair<int, int>> splitPairs;
};

/**
 * @brief Construtor de grafo a partir de webs (reutiliza regra de interferencia).
 * @param webs Lista de webs.
 * @return Grafo de interferencia.
 * @complexity O(w^2 * c) onde w = webs e c = custo de intersecao.
 */
static InterferenceGraph buildGraphFromWebs(const std::vector<Web>& webs) {
    InterferenceGraph graph;

    for (const auto& web : webs)
        graph.addNode(web.webId);

    for (size_t i = 0; i < webs.size(); i++) {
        for (size_t j = i + 1; j < webs.size(); j++) {
            if (webs[i].interferesWith(webs[j]))
                graph.addInterference(webs[i].webId, webs[j].webId);
        }
    }

    return graph;
}

/**
 * @brief Cria resultado de falha (tudo em memoria).
 * @param webs Lista de webs.
 * @return Resultado com registos 0.
 * @complexity O(w).
 */
static AllocationResult makeFailedResult(const std::vector<Web>& webs) {
    AllocationResult out;
    out.successful = false;
    out.registerCount = 0;
    out.webs = webs;
    for (const auto& web : webs)
        out.assignments.emplace_back(web.webId, web.varName);
    return out;
}

/**
 * @brief Converte cores + spills em atribuicoes.
 * @param webs Lista de webs.
 * @param colors Mapa webId -> cor.
 * @param spilled Webs a spill.
 * @param successful True se aceitavel.
 * @return Resultado pronto.
 * @complexity O(w).
 */
static AllocationResult buildResult(
    const std::vector<Web>& webs,
    const std::map<int, int>& colors,
    const std::set<int>& spilled,
    const std::vector<std::pair<int, int>>& splitPairs,
    bool successful,
    bool includeUncoloredAsSpill
) {
    AllocationResult out;
    out.successful = successful;
    out.webs = webs;
    out.splitPairs = splitPairs;

    out.spilledWebs.reserve(spilled.size());
    for (int id : spilled)
        out.spilledWebs.push_back(id);

    std::set<int> usedColors;
    for (const auto& web : webs) {
        bool isSpilled = spilled.count(web.webId) > 0;
        if (!isSpilled && includeUncoloredAsSpill && colors.count(web.webId) == 0)
            isSpilled = true;

        if (isSpilled) {
            out.assignments.emplace_back(web.webId, web.varName);
            if (std::find(out.spilledWebs.begin(), out.spilledWebs.end(), web.webId) == out.spilledWebs.end())
                out.spilledWebs.push_back(web.webId);
            continue;
        }

        auto it = colors.find(web.webId);
        if (it != colors.end()) {
            out.assignments.emplace_back(web.webId, web.varName, it->second);
            usedColors.insert(it->second);
        } else {
            out.assignments.emplace_back(web.webId, web.varName);
        }
    }

    out.registerCount = static_cast<int>(usedColors.size());
    return out;
}

/**
 * @brief Heuristica de selecao para spill/split.
 * @param graph Grafo atual.
 * @param webSizes Mapa webId -> tamanho do web.
 * @param requireSplittable Se true, ignora webs com tamanho < 2.
 * @return WebId selecionado ou -1.
 * @complexity O(V).
 */
static int chooseHighScoreWeb(
    const InterferenceGraph& graph,
    const std::map<int, int>& webSizes,
    bool requireSplittable
) {
    int bestId = -1;
    long long bestScore = -1;
    int bestDeg = -1;

    for (const auto& pair : graph.getNodes()) {
        int id = pair.first;
        int size = 1;
        auto sit = webSizes.find(id);
        if (sit != webSizes.end()) size = sit->second;
        if (requireSplittable && size < 2) continue;

        int deg = static_cast<int>(pair.second.interferesWith.size());
        long long score = static_cast<long long>(deg) * size;

        if (score > bestScore || (score == bestScore && deg > bestDeg)) {
            bestScore = score;
            bestDeg = deg;
            bestId = id;
        }
    }

    return bestId;
}

/**
 * @brief Divide um web em dois por indice de linha.
 * @param webs Lista mutavel de webs.
 * @param targetId Id do web a dividir.
 * @param nextId Proximo id disponivel.
 * @return True se dividido.
 * @complexity O(n) nas linhas do web.
 */
static bool splitWebByLines(
    std::vector<Web>& webs,
    int targetId,
    int& nextId,
    int& createdId
) {
    size_t idx = webs.size();
    for (size_t i = 0; i < webs.size(); i++)
        if (webs[i].webId == targetId) { idx = i; break; }
    if (idx == webs.size()) return false;

    Web& original = webs[idx];
    if (original.lines.size() < 2) return false;

    std::vector<int> orderedLines(original.lines.begin(), original.lines.end());
    size_t splitIndex = orderedLines.size() / 2;
    if (splitIndex == 0 || splitIndex >= orderedLines.size()) return false;

    std::set<int> leftLines;
    std::set<int> rightLines;

    for (size_t i = 0; i < orderedLines.size(); i++) {
        if (i < splitIndex) leftLines.insert(orderedLines[i]);
        else rightLines.insert(orderedLines[i]);
    }

    createdId = nextId++;
    Web right(createdId, original.varName);
    right.lines = rightLines;
    for (int line : rightLines) {
        if (original.defLines.count(line) > 0) right.defLines.insert(line);
        if (original.lastUseLines.count(line) > 0) right.lastUseLines.insert(line);
    }

    original.lines = leftLines;
    std::set<int> newDef;
    std::set<int> newLast;
    for (int line : leftLines) {
        if (original.defLines.count(line) > 0) newDef.insert(line);
        if (original.lastUseLines.count(line) > 0) newLast.insert(line);
    }
    original.defLines = newDef;
    original.lastUseLines = newLast;
    original.liveRanges.clear();

    webs.insert(webs.begin() + idx + 1, right);
    return true;
}

/**
 * @brief Executa o algoritmo de Figura 9 e aplica a regra de aceitacao do modo.
 * @param webs Lista de webs (ordem preservada no output).
 * @param graph Grafo de interferencia.
 * @param cfg Configuracao do algoritmo (basic ou spilling, K).
 * @return Resultado pronto para escrita.
 * @complexity O(V * (V + E)) dominado pela coloracao.
 */
static AllocationResult allocate(
    const std::vector<Web>& webs,
    const InterferenceGraph& graph,
    const AlgorithmConfig& cfg
) {
    if (cfg.type == AlgorithmConfig::BASIC) {
        for (int n = 1; n <= cfg.maxRegisters; n++) {
            std::map<int, int> colors;
            if (GreedyColoring::colorWithoutSpill(graph, n, colors))
                return buildResult(webs, colors, {}, {}, true, false);
        }
        return makeFailedResult(webs);
    }

    if (cfg.type == AlgorithmConfig::SPILLING) {
        std::map<int, int> webSizes;
        for (const auto& web : webs)
            webSizes[web.webId] = web.getSize();

        for (int spillsAllowed = 0; spillsAllowed <= cfg.parameter; spillsAllowed++) {
            InterferenceGraph temp = graph;
            std::set<int> spilled;

            for (int i = 0; i < spillsAllowed; i++) {
                int victim = chooseHighScoreWeb(temp, webSizes, false);
                if (victim == -1) break;
                spilled.insert(victim);
                temp.removeNode(victim);
            }

            std::map<int, int> colors;
            if (GreedyColoring::colorWithoutSpill(temp, cfg.maxRegisters, colors))
                return buildResult(webs, colors, spilled, {}, true, true);
        }

        return makeFailedResult(webs);
    }

    if (cfg.type == AlgorithmConfig::SPLITTING) {
        std::vector<Web> splitWebs = webs;
        int nextId = 0;
        for (const auto& web : splitWebs)
            if (web.webId >= nextId) nextId = web.webId + 1;

        std::vector<std::pair<int, int>> splitPairs;

        for (int splits = 0; splits <= cfg.parameter; splits++) {
            InterferenceGraph temp = buildGraphFromWebs(splitWebs);
            std::map<int, int> colors;
            if (GreedyColoring::colorWithoutSpill(temp, cfg.maxRegisters, colors))
                return buildResult(splitWebs, colors, {}, splitPairs, true, false);

            if (splits == cfg.parameter) break;

            std::map<int, int> webSizes;
            for (const auto& web : splitWebs)
                webSizes[web.webId] = web.getSize();

            int target = chooseHighScoreWeb(temp, webSizes, true);
            if (target == -1) break;
            int createdId = -1;
            if (!splitWebByLines(splitWebs, target, nextId, createdId)) break;
            splitPairs.emplace_back(target, createdId);
        }

        return makeFailedResult(splitWebs);
    }

    if (cfg.type == AlgorithmConfig::FREE) {
        ColoringResult res = GreedyColoring::runDsatur(graph, cfg.maxRegisters);
        return buildResult(webs, res.colors, res.spilled, {}, true, false);
    }

    return makeFailedResult(webs);
}

/**
 * @brief Mostra instrucoes de uso da aplicacao.
 * @param exeName Nome do executavel.
 * @complexity O(1).
 */
static void printUsage(const std::string& exeName) {
    std::cout << "Usage: " << exeName << " ranges.txt registers.txt" << std::endl;
    std::cout << "   or: " << exeName << " -b ranges.txt registers.txt output.txt" << std::endl;
    std::cout << "   or: " << exeName << " (interactive menu)" << std::endl;
}

/**
 * @brief Imprime dados parseados para debug.
 * @param parser Parser com dados carregados.
 * @complexity O(v + r) onde v = variaveis, r = total de ranges.
 */
static void printParsedData(const InputParser& parser) {
    std::cout << "\n=== PARSED DATA ===" << std::endl;
    std::cout << "Algorithm: " << parser.getAlgorithmConfig().toString() << std::endl;

    std::cout << "\nVariables and Live Ranges:" << std::endl;
    for (const auto& [varName, ranges] : parser.getVariableLiveRanges()) {
        std::cout << "  " << varName << " (" << ranges.size() << " range(s)):" << std::endl;
        for (size_t i = 0; i < ranges.size(); i++) {
            std::cout << "    Range " << i << ": [" << ranges[i].startLine
                      << " - " << ranges[i].endLine << "]";
            if (!ranges[i].defLines.empty()) std::cout << " (def)";
            if (!ranges[i].lastUseLines.empty()) std::cout << " (last_use)";
            std::cout << " Lines: ";
            for (int line : ranges[i].lines) {
                std::cout << line << " ";
            }
            std::cout << std::endl;
        }
    }
}

/**
 * @brief Imprime lista de webs.
 * @param webs Lista de webs.
 * @complexity O(w + l) onde w = webs, l = linhas totais.
 */
static void printWebs(const std::vector<Web>& webs) {
    std::cout << "\nWebs:" << std::endl;
    std::cout << "Total: " << webs.size() << std::endl;
    for (const auto& web : webs) {
        std::cout << "  " << web.toString() << " (var: " << web.varName
                  << ", size: " << web.getSize() << ")" << std::endl;
    }
}

/**
 * @brief Imprime um resumo do grafo de interferencia.
 * @param graph Grafo de interferencia.
 * @complexity O(1) para contagens pre-calculadas.
 */
static void printGraphSummary(const InterferenceGraph& graph) {
    std::cout << "\nInterference Graph:" << std::endl;
    std::cout << "Nodes: " << graph.getNumNodes() << std::endl;
    std::cout << "Edges: " << graph.getNumEdges() << std::endl;
}

/**
 * @brief Imprime atribuicoes de registos.
 * @param assignments Lista de atribuicoes.
 * @param registerCount Numero de registos usados.
 * @complexity O(a) onde a = numero de atribuicoes.
 */
static void printAssignments(
    const std::vector<RegisterAssignment>& assignments,
    int registerCount
) {
    std::cout << "\nRegisters: " << registerCount << std::endl;
    for (const auto& assignment : assignments) {
        std::cout << "  " << assignment.toString() << std::endl;
    }
}

/**
 * @brief Escreve ficheiro de output com webs e atribuicoes.
 * @param outputFile Caminho do ficheiro de saida.
 * @param webs Lista de webs.
 * @param registerCount Numero de registos atribuiveis.
 * @param assignments Atribuicoes de registos.
 * @return True se escrito com sucesso.
 * @complexity O(w + a) onde w = webs e a = atribuicoes.
 */
static bool writeOutputFile(const std::string& outputFile, const AllocationResult& result) {
    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "ERROR: Cannot write output file: " << outputFile << std::endl;
        return false;
    }

    out << "webs: " << result.webs.size() << "\n";
    for (const auto& web : result.webs) {
        out << web.toString() << "\n";
    }

    if (!result.spilledWebs.empty()) {
        out << "\nspills: " << result.spilledWebs.size() << "\n";
        for (size_t i = 0; i < result.spilledWebs.size(); i++) {
            out << "spill" << i << ": web" << result.spilledWebs[i] << "\n";
        }
    }

    if (!result.splitPairs.empty()) {
        out << "\nsplits: " << result.splitPairs.size() << "\n";
        for (size_t i = 0; i < result.splitPairs.size(); i++) {
            out << "split" << i << ": web" << result.splitPairs[i].first
                << " -> web" << result.splitPairs[i].first
                << ", web" << result.splitPairs[i].second << "\n";
        }
    }

    out << "\nregisters: " << result.registerCount << "\n";
    for (const auto& assignment : result.assignments) {
        out << assignment.toString() << "\n";
    }

    return true;
}

/**
 * @brief Executa o batch mode.
 * @param rangesFile Ficheiro ranges.txt.
 * @param registersFile Ficheiro registers.txt.
 * @param outputFile Ficheiro de output.
 * @return True se tudo correr bem.
 * @complexity Dominado pelo parsing e build de webs.
 */
static bool runBatch(
    const std::string& rangesFile,
    const std::string& registersFile,
    const std::string& outputFile
) {
    InputParser parser;

    std::cout << "[1] Parsing ranges from: " << rangesFile << std::endl;
    if (!parser.parseRanges(rangesFile)) {
        std::cerr << "FATAL: Failed to parse ranges" << std::endl;
        return false;
    }

    std::cout << "[2] Parsing registers from: " << registersFile << std::endl;
    if (!parser.parseRegisters(registersFile)) {
        std::cerr << "FATAL: Failed to parse registers" << std::endl;
        return false;
    }

    auto webs = parser.buildWebs();
    auto graph = parser.buildInterferenceGraph(webs);
    printGraphSummary(graph);

    auto allocation = allocate(webs, graph, parser.getAlgorithmConfig());
    if (!allocation.successful)
        std::cerr << "WARNING: allocation not possible." << std::endl;

    if (!writeOutputFile(outputFile, allocation))
        return false;

    std::cout << "Batch output written to: " << outputFile << std::endl;
    return true;
}

/**
 * @brief Menu interativo para testar parsing e output.
 * @complexity Dependente das operacoes escolhidas pelo utilizador.
 */
static void interactiveMenu() {
    InputParser parser;
    bool hasRanges = false;
    bool hasRegisters = false;
    bool hasWebs = false;
    std::vector<Web> webs;

    while (true) {
        std::cout << "\n=== MENU ===" << std::endl;
        std::cout << "1) Parse ranges.txt" << std::endl;
        std::cout << "2) Parse registers.txt" << std::endl;
        std::cout << "3) Build webs" << std::endl;
        std::cout << "4) Show parsed data" << std::endl;
        std::cout << "5) Save batch output" << std::endl;
        std::cout << "0) Exit" << std::endl;
        std::cout << "> ";

        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            std::cout << "Invalid input." << std::endl;
            continue;
        }

        if (choice == 0) {
            break;
        }

        if (choice == 1) {
            std::string rangesFile;
            std::cout << "Ranges file path: ";
            std::cin >> rangesFile;
            hasRanges = parser.parseRanges(rangesFile);
            if (hasRanges) std::cout << "\u2713 Ranges parsed." << std::endl;
        } else if (choice == 2) {
            std::string registersFile;
            std::cout << "Registers file path: ";
            std::cin >> registersFile;
            hasRegisters = parser.parseRegisters(registersFile);
            if (hasRegisters) std::cout << "\u2713 Registers parsed." << std::endl;
        } else if (choice == 3) {
            if (!hasRanges) {
                std::cout << "Parse ranges first." << std::endl;
                continue;
            }
            webs = parser.buildWebs();
            hasWebs = true;
            printWebs(webs);
            auto graph = parser.buildInterferenceGraph(webs);
            printGraphSummary(graph);
        } else if (choice == 4) {
            if (!hasRanges || !hasRegisters) {
                std::cout << "Parse ranges and registers first." << std::endl;
                continue;
            }
            printParsedData(parser);
        } else if (choice == 5) {
            if (!hasRanges || !hasRegisters) {
                std::cout << "Parse ranges and registers first." << std::endl;
                continue;
            }

            if (!hasWebs) {
                webs = parser.buildWebs();
                hasWebs = true;
            }

            std::string outputFile;
            std::cout << "Output file path: ";
            std::cin >> outputFile;

            auto graph = parser.buildInterferenceGraph(webs);
            auto allocation = allocate(webs, graph, parser.getAlgorithmConfig());
            if (!allocation.successful)
                std::cerr << "err: allocation not possible." << std::endl;
            if (writeOutputFile(outputFile, allocation))
                std::cout << "Output written to: " << outputFile << std::endl;
        } else {
            std::cout << "Unknown option." << std::endl;
        }
    }
}

/**
 * @brief Programa main para interface e batch mode.
 * @param argc Numero de argumentos.
 * @param argv Argumentos de linha de comando.
 * @return Codigo de saida.
 * @complexity O(1) fora chamadas auxiliares.
 */
int main(int argc, char* argv[]) {
    if (argc == 1) {
        interactiveMenu();
        return 0;
    }

    if (argc >= 2 && std::string(argv[1]) == "-b") {
        if (argc < 5) {
            printUsage(argv[0]);
            return 1;
        }
        if (!runBatch(argv[2], argv[3], argv[4])) {
            return 1;
        }
        return 0;
    }

    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    InputParser parser;

    std::cout << "[1] Parsing ranges from: " << argv[1] << std::endl;
    if (!parser.parseRanges(argv[1])) {
        std::cerr << "FATAL: Failed to parse ranges" << std::endl;
        return 1;
    }

    std::cout << "\n[2] Parsing registers from: " << argv[2] << std::endl;
    if (!parser.parseRegisters(argv[2])) {
        std::cerr << "FATAL: Failed to parse registers" << std::endl;
        return 1;
    }

    printParsedData(parser);

    std::cout << "\n[3] Building webs..." << std::endl;
    auto webs = parser.buildWebs();
    printWebs(webs);

    auto graph = parser.buildInterferenceGraph(webs);
    printGraphSummary(graph);

    auto allocation = allocate(webs, graph, parser.getAlgorithmConfig());
    if (!allocation.successful)
        std::cout << "WARNING: allocation not possible." << std::endl;
    printAssignments(allocation.assignments, allocation.registerCount);

    return 0;
}