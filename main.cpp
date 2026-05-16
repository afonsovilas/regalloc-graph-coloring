/**
 * @file main.cpp
 * @brief Interface CLI e batch mode do projeto.
 */

#include <iostream>
#include <fstream>
#include <set>
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
};

/**
 * @brief Executa o algoritmo de Figura 9 e aplica a regra de aceitacao do modo.
 * @param webs Lista de webs (ordem preservada no output).
 * @param graph Grafo de interferencia.
 * @param cfg Configuracao do algoritmo (basic ou spilling, K).
 * @return Resultado pronto para escrita.
 * @complexity O(V * (V + E)) dominado pela coloracao.
 */
static AllocationResult allocate(const std::vector<Web>& webs,const InterferenceGraph& graph,const AlgorithmConfig& cfg) {
    ColoringResult coloring = GreedyColoring::run(graph, cfg.maxRegisters);

    bool accepted = false;
    if (cfg.type == AlgorithmConfig::BASIC)
        accepted = coloring.spilled.empty();
    else if (cfg.type == AlgorithmConfig::SPILLING)
        accepted = static_cast<int>(coloring.spilled.size()) <= cfg.parameter;

    AllocationResult out;
    out.successful = accepted;

    if (!accepted) {
        out.registerCount = 0;
        for (const auto& web : webs)
            out.assignments.emplace_back(web.webId, web.varName);
        return out;
    }

    std::set<int> usedColors;
    for (const auto& web : webs) {
        auto it = coloring.colors.find(web.webId);
        if (it != coloring.colors.end()) {
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
            if (ranges[i].isDefinition) std::cout << " (def)";
            if (ranges[i].isLastUse) std::cout << " (last_use)";
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
 * @brief Escreve ficheiro de output com webs e atribuicoes.
 * @param outputFile Caminho do ficheiro de saida.
 * @param webs Lista de webs.
 * @param registerCount Numero de registos atribuiveis.
 * @param assignments Atribuicoes de registos.
 * @return True se escrito com sucesso.
 * @complexity O(w + a) onde w = webs e a = atribuicoes.
 */
static bool writeOutputFile(
    const std::string& outputFile,
    const std::vector<Web>& webs,
    int registerCount,
    const std::vector<RegisterAssignment>& assignments
) {
    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "ERROR: Cannot write output file: " << outputFile << std::endl;
        return false;
    }

    out << "webs: " << webs.size() << "\n";
    for (const auto& web : webs) {
        out << web.toString() << "\n";
    }

    out << "\nregisters: " << registerCount << "\n";
    for (const auto& assignment : assignments) {
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
    if (!allocation.successful) {
        std::cerr << "WARNING: Allocation with "
                  << parser.getAlgorithmConfig().maxRegisters
                  << " registers was not possible." << std::endl;
    }

    if (!writeOutputFile(outputFile, webs, allocation.registerCount, allocation.assignments))
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
            if (hasRanges) std::cout << "✓ Ranges parsed." << std::endl;
        } else if (choice == 2) {
            std::string registersFile;
            std::cout << "Registers file path: ";
            std::cin >> registersFile;
            hasRegisters = parser.parseRegisters(registersFile);
            if (hasRegisters) std::cout << "✓ Registers parsed." << std::endl;
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

            std::vector<RegisterAssignment> assignments;
            assignments.reserve(webs.size());
            for (const auto& web : webs) {
                assignments.emplace_back(web.webId, web.varName);
            }

            int registerCount = 0;
            if (writeOutputFile(outputFile, webs, registerCount, assignments)) {
                std::cout << "✓ Output written to: " << outputFile << std::endl;
            }
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

    return 0;
}
