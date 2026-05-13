# Resumo Rápido - Divisão em 3 Pessoas

## 📌 Visão Geral (20 pontos totais)

```
┌─────────────────────────────────────────────────────────────┐
│        PROJETO: Compiler Register Allocation                │
│  Objetivo: Implementar alocação de registos via graph       │
│           coloring (NP-complete problem)                    │
└─────────────────────────────────────────────────────────────┘
```

---

## 🎯 Divisão de Trabalho

### **PESSOA 1️⃣ : Infrastructure Lead (4 pontos)**

```
┌──────────────────────────────────────────┐
│   APLICAÇÃO & INTERFACE (T1.x)           │
├──────────────────────────────────────────┤
│ T1.1 Menu interativo (1 pt)              │
│  └─ Batch mode: myProg -b in1 in2 out   │
│  └─ Menu interativo com opções           │
│                                          │
│ T1.2 Parser Input (1 pt)                 │
│  └─ ranges.txt → variáveis + liveRanges │
│  └─ registers.txt → K registos           │
│  └─ Estruturas de dados                  │
│                                          │
│ T1.3 Documentação (2 pts)                │
│  └─ Doxygen comments                     │
│  └─ Análise complexidade O(...)          │
│  └─ Plus: Output formatter               │
└──────────────────────────────────────────┘
   Dependencies: Pessoa 2 & 3 → estruturas
```

---

### **PESSOA 2️⃣ : Data Structures (Web & Graph)**

```
┌──────────────────────────────────────────┐
│   WEB & INTERFERENCE GRAPH               │
├──────────────────────────────────────────┤
│ ⚙️  WebBuilder                           │
│  └─ Input: ranges de mesma variável      │
│  └─ Task: Merge ranges overlapping       │
│  └─ Output: Webs únicos                  │
│  └─ Ex: i: 1-6, i: 9-12 → 2 webs       │
│                                          │
│ ⚙️  InterferenceGraph                    │
│  └─ Nodes: webs                          │
│  └─ Edges: webs que coexistem            │
│  └─ CUIDADO: semântica de "fim" vs      │
│               "início" na mesma linha    │
│  └─ Output: Graph pronto para coloring   │
│                                          │
│ ✅ Testes com casos do PDF               │
└──────────────────────────────────────────┘
   Depends on: Pessoa 1 parser
   Feeds to: Pessoa 3 algorithms
```

---

### **PESSOA 3️⃣ : Graph Coloring Algorithms (12 pontos)**

```
┌──────────────────────────────────────────┐
│   ALGORITMOS DE ALOCAÇÃO                 │
├──────────────────────────────────────────┤
│ T2.1 Basic Coloring (4 pts)              │
│  └─ Greedy algorithm (Figure 9 do PDF)   │
│  └─ Fases: Remove cliques → Spill → Color│
│  └─ Output: Atribuição ou FALHA          │
│                                          │
│ T2.2 Web Spilling (3 pts)                │
│  └─ Se basic falha: spill até K webs    │
│  └─ Strategy: maior grau? menor tamanho?│
│  └─ Iterativo: retry até sucesso        │
│  └─ Output: webs → registos ou M        │
│                                          │
│ T2.3 Web Splitting (3 pts)               │
│  └─ Se basic falha: split até K webs    │
│  └─ Strategy: pares/ímpares? por ponto? │
│  └─ Reconstrói graph + retry            │
│  └─ Output: sub-webs com registos        │
│                                          │
│ T2.4 Custom Algorithm (4 pts)            │
│  └─ Tua ideia! (heurística melhorada,   │
│     coalescing, dynamic split+spill)    │
│  └─ Compare com basic                    │
└──────────────────────────────────────────┘
   Input: Interference graph da Pessoa 2
   Output: Resultados ficheiro (Pessoa 1)
```

---

## 🔄 Fluxo de Dados (Integração)

```
ranges.txt, registers.txt
         │
         ↓
    [PESSOA 1: Parser]
         │
         ├──→ LiveRanges, K registos, algoritmo choice
         │
         ↓
    [PESSOA 2: Web Construction]
         │
         ├──→ Webs[], InterferenceGraph
         │
         ↓
    [PESSOA 3: Graph Coloring]
         │
         ├──→ Coloring[], spilledWebs[], splitWebs[]
         │
         ↓
    [PESSOA 1: Output Formatter]
         │
         ↓
    output.txt (webs + register assignments)
```

---

## 📋 Checklist por Pessoa

### ✅ PESSOA 1 - Infrastructure
- [ ] Main.cpp com menu interativo
- [ ] Batch mode parser argumentos
- [ ] InputParser classe (parse ranges & registers)
- [ ] OutputFormatter classe (escrever output.txt)
- [ ] Graph classe base (adaptar do TP)
- [ ] DataStructures.h (LiveRange, Web, etc)
- [ ] Doxygen comments em tudo
- [ ] Compilação sem erros
- [ ] Testes funcionalidade I/O

### ✅ PESSOA 2 - Webs & Graph
- [ ] WebBuilder classe (merge logic)
- [ ] Testes merge com casos simples
- [ ] InterferenceGraph classe (extends Graph)
- [ ] Implementar interferência (com semântica correta)
- [ ] Testes com casos PDF (Figure 5, 6)
- [ ] Visualização opcional (debug)
- [ ] Doxygen comments

### ✅ PESSOA 3 - Algorithms
- [ ] **T2.1:** greedyColoring() funcionando básico
- [ ] Testar T2.1 com K=2,3,4 em casos simples
- [ ] **T2.2:** spillingColoring() com strategy
- [ ] Testar spilling em casos que falham
- [ ] **T2.3:** splittingColoring() com strategy
- [ ] Testar splitting em casos que falham
- [ ] **T2.4:** Custom algorithm com documentação
- [ ] Comparação: básico vs T2.2 vs T2.3 vs T2.4
- [ ] Doxygen comments + complexidade

---

## 📅 Timeline (5 semanas até Deadline)

| Fase | Pessoa 1 | Pessoa 2 | Pessoa 3 |
|------|----------|----------|----------|
| **Semana 1** | Parser básico | WebBuilder initial | Understand greedy algo |
| **Semana 2** | Menu + I/O formatters | InterferenceGraph | Implement T2.1 |
| **Semana 3** | Integração teste | Bug fixes + testing | T2.2 Spilling |
| **Semana 4** | Doxygen + docs | Final validation | T2.3/T2.4 Custom |
| **Semana 5** | Final review | Optimizations | Demo + presentation |

---

## 🧪 Exemplo End-to-End

**Input Files:**

`ranges.txt`:
```
sum: 1+,2,3-
i: 2+,3,4,5,6-
t0: 3+,4,5-
```

`registers.txt`:
```
registers: 2
algorithm: basic
```

**Expected Flow:**

1. **Pessoa 1 parses:** 
   - sum: {1+, 2, 3-}
   - i: {2+, 3, 4, 5, 6-}
   - t0: {3+, 4, 5-}

2. **Pessoa 2 builds webs & graph:**
   - Web 0: sum {1, 2, 3}
   - Web 1: i {2, 3, 4, 5, 6}
   - Web 2: t0 {3, 4, 5}
   - Edges: (0-1), (1-2), (0-2) [todos interferem em linha 3]
   - Clique tamanho 3 → precisa 3 cores!

3. **Pessoa 3 tenta coloring:**
   - K=2: FALHA (clique de tamanho 3)
   - Output: "Assignment to 2 registers was not possible"
   - Result: registers: 0, M: web0, M: web1, M: web2

4. **Pessoa 1 outputs:**
```
webs: 3
web0: 1+,2,3-
web1: 2+,3,4,5,6-
web2: 3+,4,5-

registers: 0
M: web0
M: web1
M: web2
```

---

## 🎓 Conceitos Críticos

| Conceito | Explicação | Implementador |
|----------|-----------|---|
| **Live Range** | Linhas onde var está "alive" (pode ser usada) | Pessoa 1 parser |
| **Web** | União de live ranges overlapping | Pessoa 2 |
| **Interference** | Dois webs coexistem num ponto | Pessoa 2 |
| **Coloring** | Atribuir cores (registos) sem conflito | Pessoa 3 |
| **Spilling** | Deixar web em memory (M) se não cabe | Pessoa 3 |
| **Splitting** | Partir web em sub-webs | Pessoa 3 |

---

## 🚀 Dicas de Implementação

1. **Escolham linguagem:** C++ (recomendado), Java, ou Python
2. **Graph:** Usem lista/matriz adjacência. Adaptem do TP!
3. **Testing:** Facades de testes - cada pessoa testa seu módulo
4. **Integration:** Reuniões 2x semana para sincronizar
5. **Demo:** Preparem 3-4 cases interessantes (básico, spilling, splitting, custom)
6. **Presentation:** 10 min = ~7-8 slides PowerPoint

---

## 📞 Coordenação

- **Pessoa 1** = Coordenadora (integra tudo)
- **Reuniões:** 2x semana (terça + sexta recomendado)
- **Git:** Sempre com branches (pessoa1/, pessoa2/, pessoa3/)
- **Bugs:** Log e report no README.md

