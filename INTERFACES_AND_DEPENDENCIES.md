# Interfaces & Dependências Entre Componentes

## 📌 Estrutura de Módulos

```
┌─────────────────────────────────────────────────────────────────┐
│                        APLICAÇÃO PRINCIPAL                      │
│                      (Pessoa 1: Main.cpp)                       │
└─────────────────────────────────────────────────────────────────┘
         ↓                    ↓                    ↓
    ┌────────────┐    ┌─────────────┐    ┌──────────────────┐
    │  InputMgr  │    │   Engine    │    │  OutputFormatter │
    │(Pessoa 1)  │    │(Pessoa 3)   │    │   (Pessoa 1)     │
    └────┬───────┘    └──────┬──────┘    └───────┬──────────┘
         │                   │                   │
         └───────────────────┼───────────────────┘
                             ↓
            ┌────────────────────────────────────┐
            │   WebBuilder & InterferenceGraph   │
            │         (Pessoa 2)                 │
            └────────────────────────────────────┘
                             ↓
                     ┌─────────────┐
                     │ Graph Base  │
                     │(all use it) │
                     └─────────────┘
```

---

## 🔌 Interfaces Principais

### **Interface 1: InputParser → WebBuilder**

**Output do Parser (Pessoa 1):**
```cpp
struct LiveRange {
    int start;
    int end;
    bool isStart;  // + marker
    bool isEnd;    // - marker
};

struct VariableLiveRanges {
    string varName;
    vector<LiveRange> ranges;  // múltiplos ranges por variável
};

struct InputData {
    map<string, VariableLiveRanges> allVariables;
    int numRegisters;
    string algorithm;  // "basic", "spilling", "splitting", "free"
    int algorithmParam;  // K para spilling/splitting (opcional)
};
```

**Input do WebBuilder (Pessoa 2):**
```cpp
// Recebe InputData acima
WebList buildWebs(InputData input) {
    // Processa ranges
    // Retorna webs
}
```

---

### **Interface 2: WebBuilder → InterferenceGraph**

**Output do WebBuilder:**
```cpp
struct Web {
    int webId;
    string originalVar;  // qual variável original
    set<int> programPoints;  // linhas onde está alive
    bool isSpilled = false;  // se for T2.2/T2.3
};

struct WebList {
    vector<Web> webs;
    int totalWebs() const { return webs.size(); }
    Web getWeb(int id) const { return webs[id]; }
};
```

**Input do InterferenceGraph (Pessoa 2 depois):**
```cpp
// Recebe WebList
InterferenceGraph buildGraph(WebList webList) {
    // Para cada par (web_i, web_j):
    //   se existe ponto onde ambos existem → edge
    // Retorna graph
}

// O graph tem:
// - Nodes: 0..numWebs-1 (cada node = um web)
// - Edges: lista de adjacência
// - Métodos: getNeighbors(node), getDegree(node), etc
```

---

### **Interface 3: InterferenceGraph → GraphColoring**

**Output do InterferenceGraph (Pessoa 2):**
```cpp
class InterferenceGraph : public Graph {
    // Extends Graph base
    
    // Métodos básicos:
    int getNumNodes() const;
    vector<int> getNeighbors(int node) const;
    int getDegree(int node) const;
    
    // Métodos auxiliares:
    int getMaxDegree() const;
    vector<pair<int,int>> getAllEdges() const;
    bool hasEdge(int u, int v) const;
    
    // Para debugging:
    void printGraph() const;
    void visualize(string filename) const;  // opcional
};
```

**Input do GraphColoring (Pessoa 3):**
```cpp
// Input: (InterferenceGraph, K)
// Output: colorAssignment[] ou FALHA

struct ColoringResult {
    bool success;
    vector<int> assignment;  // assignment[node] = color (0..K-1)
    set<int> spilledNodes;   // nós spilled (se T2.2)
    set<pair<int,int>> splitNodes;  // sub-webs criados (se T2.3)
};

ColoringResult greedyColoring(
    const InterferenceGraph& graph, 
    int K
) {
    // Retorna resultado
}
```

---

### **Interface 4: GraphColoring → OutputFormatter**

**Output do GraphColoring (Pessoa 3):**
```cpp
struct AllocationResult {
    bool feasible;  // conseguiu colorir?
    int registersUsed;
    map<int, int> webToRegister;  // web_id → register_id (0..K-1)
    set<int> spilledWebs;  // web_ids que foram para memory
    
    // Metadata:
    string algorithmUsed;  // "basic", "spilling", etc
    int spillCount;  // quantos foram spilled
    int splitCount;  // quantos foram split
    double executionTime;  // ms
};
```

**Input do OutputFormatter (Pessoa 1):**
```cpp
// Recebe AllocationResult + WebList
void writeOutput(
    const string& filename,
    const WebList& webs,
    const AllocationResult& result
) {
    // Formato específico do PDF (Figure 11/12)
}
```

---

## 📊 Comunicação Entre Pessoas

### **Pessoa 1 ↔ Pessoa 2**

```
Pessoa 1 parse ficheiro:
    ranges.txt → InputData
              ↓
Passa para Pessoa 2:
    InputData

Pessoa 2 processa:
    WebList buildWebs(InputData)
              ↓
Retorna para Pessoa 1 (se necessário):
    WebList (opcional - Pessoa 1 só chama Pessoa 2 via Engine)
```

### **Pessoa 2 ↔ Pessoa 3**

```
Pessoa 2 constrói graph:
    InterferenceGraph graph = buildGraph(webList)
              ↓
Passa para Pessoa 3:
    InterferenceGraph

Pessoa 3 colore:
    ColoringResult = greedyColoring(graph, K)
              ↓
Retorna para Pessoa 1:
    AllocationResult (via Engine)
```

### **Pessoa 1 ↔ Pessoa 3**

```
Pessoa 1 conhece:
    - InputData (parsed ficheiros)
    - Algorithm choice

Pessoa 3 (Engine):
    - Escolhe algoritmo
    - Chama greedyColoring() ou spillingColoring() etc
    - Retorna AllocationResult

Pessoa 1 (Main):
    - Chama Pessoa 3 com InputData
    - Recebe AllocationResult
    - Formata output
```

---

## 🔧 Header Files que Precisam Concordar

### **Ficheiro: DataStructures.h** (Compartilhado)
```cpp
#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <vector>
#include <set>
#include <map>
#include <string>

// Pessoa 1 + 2 + 3 usam isto!

struct LiveRange {
    int start;
    int end;
    bool isStart;
    bool isEnd;
};

struct Web {
    int webId;
    string originalVar;
    set<int> programPoints;
    bool isSpilled;
};

struct WebList {
    vector<Web> webs;
    // métodos comuns
};

struct ColoringResult {
    bool success;
    vector<int> assignment;
    set<int> spilledNodes;
};

#endif
```

---

## 📋 Checklist de Integração

### Semana 1-2 (Setup)
- [ ] Todas: Definir DataStructures.h (reunião!)
- [ ] Pessoa 1: Criar estrutura projeto (Makefile/CMake)
- [ ] Todas: Git setup com branches

### Semana 2 (Alpha)
- [ ] Pessoa 1: InputParser funcional
  - [ ] Testa com ranges.txt de exemplo
  - [ ] Retorna InputData correto
- [ ] Pessoa 2: WebBuilder inicial
  - [ ] Testa merge simples
  - [ ] Verifica output WebList
- [ ] Pessoa 3: greedyColoring inicial
  - [ ] Testa com graph simples (3 nodes, 2 edges)

### Semana 3 (Integration)
- [ ] Pessoa 1+2: InputData → WebList funciona
  - [ ] Main.cpp chama buildWebs()
  - [ ] Valida WebList output
- [ ] Pessoa 2+3: WebList → InterferenceGraph funciona
  - [ ] buildGraph() retorna graph válido
  - [ ] GraphColoring consegue usar
- [ ] Pessoa 1+3: GraphColoring → Output funciona
  - [ ] AllocationResult → output.txt

### Semana 4 (Polish)
- [ ] T2.2 (Spilling) integrado
- [ ] T2.3 (Splitting) integrado
- [ ] T2.4 (Custom) integrado
- [ ] Todos os ficheiros têm Doxygen
- [ ] Sem memory leaks/crashes

### Semana 5 (Demo)
- [ ] Batch mode funciona
- [ ] Menu interativo funciona
- [ ] Casos de teste preparados
- [ ] Presentation.pdf pronto

---

## 🐛 Common Integration Issues

| Issue | Root | Solução |
|-------|------|---------|
| WebList não tem método X | Interfacce incompleta | Adicionar DataStructures.h |
| Graph nodes não correspondem | Indexação diferente | Usar webId como node id |
| Interference graph vazio | Web building errado | Debug: imprimir webs antes |
| Coloring retorna lixo | ColoringResult mal inicializado | Validar assignment vector |
| Output ficheiro vazio | OutputFormatter não chamado | Verificar fluxo main |

---

## 📞 Reunião Weekly Template

```
REUNIÃO INTEGRAÇÃO - Sexta 3PM

1. STATUS (5 min)
   Pessoa 1: Parser 80%, Menu 40%
   Pessoa 2: WebBuilder 60%, Graph 50%
   Pessoa 3: greedyColoring 90%, T2.2 0%

2. BLOCKERS (10 min)
   Pessoa X: Preciso de Y da Pessoa Z
   → Resolver

3. TESTE END-TO-END (15 min)
   Rodar: myProg -b test1.txt test_regs.txt out.txt
   Verificar: output.txt está correto?

4. PRÓXIMA SEMANA (5 min)
   Pessoa 1: Implementar X
   Pessoa 2: Implementar Y
   Pessoa 3: Implementar Z
   → Reunir quarta para discussão
```

---

## 🎯 Validação de Interface

Para confirmar que interface está correta:

```cpp
// Test.cpp (Pessoa 1 testa integração)
#include "DataStructures.h"
#include "InputParser.h"
#include "WebBuilder.h"
#include "InterferenceGraph.h"
#include "GraphColoring.h"

int main() {
    // Testa Pessoa 1
    InputData input = parseInput("test.txt", "regs.txt");
    assert(input.allVariables.size() > 0);
    
    // Testa Pessoa 2
    WebList webs = buildWebs(input);
    assert(webs.totalWebs() > 0);
    
    InterferenceGraph graph = buildGraph(webs);
    assert(graph.getNumNodes() == webs.totalWebs());
    
    // Testa Pessoa 3
    ColoringResult result = greedyColoring(graph, input.numRegisters);
    assert(result.assignment.size() == webs.totalWebs());
    
    // Testa Pessoa 1 output
    writeOutput("out.txt", webs, result);
    
    cout << "✅ All integrations OK!" << endl;
    return 0;
}
```

