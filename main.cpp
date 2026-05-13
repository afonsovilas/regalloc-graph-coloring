#include <iostream>
#include "include/parsers/InputParser.h"

/**
 * @brief Programa main para testar InputParser
 * 
 * Uso:
 * ./main ranges.txt registers.txt
 * 
 * Ou:
 * ./main -b ranges.txt registers.txt output.txt (batch mode - TODO)
 */
int main(int argc, char* argv[]) {
    
    // Verificar argumentos
    if (argc < 3) {
        std::cout << "Usage: ./main ranges.txt registers.txt" << std::endl;
        std::cout << "   or: ./main -b ranges.txt registers.txt output.txt" << std::endl;
        return 1;
    }

    InputParser parser;

    // Parse ficheiro de ranges
    std::cout << "[1] Parsing ranges from: " << argv[1] << std::endl;
    if (!parser.parseRanges(argv[1])) {
        std::cerr << "FATAL: Failed to parse ranges" << std::endl;
        return 1;
    }
    std::cout << "✓ Ranges parsed successfully" << std::endl;

    // Parse ficheiro de registos
    std::cout << "\n[2] Parsing registers from: " << argv[2] << std::endl;
    if (!parser.parseRegisters(argv[2])) {
        std::cerr << "FATAL: Failed to parse registers" << std::endl;
        return 1;
    }
    std::cout << "✓ Registers parsed successfully" << std::endl;

    // Mostrar resultado
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

    // Build webs
    std::cout << "\n[3] Building webs..." << std::endl;
    auto webs = parser.buildWebs();
    std::cout << "✓ " << webs.size() << " web(s) created:" << std::endl;
    for (const auto& web : webs) {
        std::cout << "  " << web.toString() << " (var: " << web.varName 
                  << ", size: " << web.getSize() << ")" << std::endl;
    }

    return 0;
}
