# Divisão do Projeto - Register Allocation (3 Pessoas)

## 📋 Visão Geral do Projeto

**Objectivo:** Implementar uma ferramenta de alocação de registos em compiladores usando graph coloring.

**Deadline:** 17 Maio 2026, 23:59 (PT)  
**Total:** 20 pontos

---

## 👥 Divisão em 3 Partes

### **PESSOA 1: Infrastructure & Core Framework (Coordenador)**
**Responsabilidade:** Base de tudo - menu, parsing, estruturas de dados, output

**Tarefas:**
- ✅ **T1.1 (1.0 pt):** Desenvolver aplicação
  - Menu interativo com opções para:
    - Carregar ficheiros
    - Escolher algoritmo (basic/spilling/splitting/free)
    - Visualizar resultados
  - Interface batch: `myProg -b ranges.txt registers.txt output.txt`
  - Tratamento de erros

- ✅ **T1.2 (1.0 pt):** Parser de Input
  - Parse `ranges.txt`: variáveis → live ranges
  - Parse `registers.txt`: número de registos + algoritmo
  - Estruturas de dados para armazenar dados
  - Validação de input

- ✅ **T1.3 (2.0 pts):** Documentação & Análise
  - Documentação Doxygen de todos os modules
  - Análise de complexidade para algoritmos principais
  - README técnico

- ✅ **Plus:** Formatter de Output
  - Escrever ficheiro output com formato especificado
  - Webs com program points ordenados
  - Atribuição de registos (r0, r1, ... ou M para memory)

**Arquivos/Classes:**
```
- Main.cpp / main.py
- InputParser.h/cpp
- OutputFormatter.h/cpp
- Graph.h/cpp (estrutura base - pode adaptar do TP)
- DataStructures.h (definições de Web, LiveRange, etc)
```

**Entregas:**
- Aplicação compilada/executável
- Documentação Doxygen (pasta Documentation)
- Ficheiro README

---

### **PESSOA 2: Web Construction & Interference Graph**
**Responsabilidade:** Do parsing bruto para estruturas lógicas (webs e graph)

**Tarefas:**
- ✅ **Construção de Webs**
  - Recebe: lista de live ranges por variável
  - Problema: mesma variável pode ter múltiplos live ranges que se sobrepõem
  - Solução: Merge ranges overlapping em "webs"
  - Algoritmo: greedy merge (verificar overlap)
  - Output: Lista de webs com todos os program points

  Exemplo:
  ```
  Input:  i: 1+,2,3,4,5,6-
          i: 9+,10,11,12-
  Output: web_i_1: 1+,2,3,4,5,6-
          web_i_2: 9+,10,11,12-
  ```

- ✅ **Construção do Interference Graph**
  - Nós = webs (web_0, web_1, web_2, ...)
  - Arestas = webs que interferem (coexistem em algum programa point)
  - **Regra importante:** Web A (def em linha X, uso em linha Y) e Web B (def em linha Y) NÃO interferem!
    - Implementar corretamente esta semântica
  - Output: Grafo com matriz/lista de adjacência

- ✅ **Testes & Validação**
  - Verificar webs com dados de exemplo do PDF
  - Plotar/visualizar interference graph (opcional mas útil)

**Algoritmos Principais:**
- Merge de live ranges (complexidade?)
- Deteção de interferência (O(n²) onde n=número de webs)
- Construção de grafo (O(n + m) onde m=arestas)

**Arquivos/Classes:**
```
- WebBuilder.h/cpp
  - mergeRanges()
  - constructWebs()
  
- InterferenceGraph.h/cpp (extends Graph)
  - isInterfering(web_i, web_j)
  - buildInterferenceGraph()
  - getInterferenceEdges()
```

**Entregas:**
- Classe WebBuilder totalmente funcional
- Classe InterferenceGraph
- Testes unitários para casos do PDF

---

### **PESSOA 3: Graph Coloring & Advanced Algorithms**
**Responsabilidade:** Algoritmos de alocação de registos (o "coração" do projeto)

**Tarefas:**

- ✅ **T2.1 (4.0 pts): Basic Graph Coloring**
  - Implementar greedy algorithm do PDF (Figure 9)
  - Entrada: Interference Graph + número de cores (K registos)
  - Fases:
    1. **Clique removal:** Remove nós com grau < K até saturação
    2. **Spilling:** Se todos têm grau ≥ K, spill um nó (push stack)
    3. **Coloring:** Pop stack, atribuir cor diferente de vizinhos
  - Saída: True/False se conseguiu colorir com K cores
  - Análise de complexidade: O(n + m)

- ✅ **T2.2 (3.0 pts): Web Spilling**
  - Quando basic falha com K registos, permitir spill de até M webs
  - Estratégia de seleção de qual web spill:
    - Opção 1: Web com maior grau (tira mais arestas)
    - Opção 2: Web de menor tamanho (menos impacto)
    - **Documentar escolha!**
  - Algoritmo:
    1. Tenta basic coloring com K
    2. Se falha, remove web com score mais alto
    3. Repete até conseguir ou esgotar limite M de spills
  - Saída: Lista de webs spilled (alocadas em memory "M")

- ✅ **T2.3 (3.0 pts): Web Splitting**
  - Quando basic falha, particionar webs em sub-webs
  - Splitring strategy:
    - Opção 1: Split em dois (linhas pares vs ímpares)
    - Opção 2: Split no ponto de maior "interferência"
    - **Documentar escolha!**
  - Algoritmo:
    1. Tenta basic coloring
    2. Se falha, split web com score mais alto
    3. Reconstrói interference graph com sub-webs
    4. Repete até conseguir ou esgotar limite M de splits
  - Saída: Sub-webs com atribuições

- ✅ **T2.4 (4.0 pts): Algoritmo Próprio**
  - Sugestões:
    - Coloring heurístico melhorado (ex: best-fit em vez de first-fit)
    - Combinar spilling + splitting dinamicamente
    - Usar métricas de grafo (planarity, articulation points)
    - Live range coalescing (merge webs com mesmo padrão)
  - **Documentar bem a ideia e comparar com basic**

**Algoritmos & Análise:**
- Basic coloring: O(n + m)
- Spilling: O(k * (n + m)) onde k = tentativas
- Splitting: O(k * (n + m))
- Custom: depende da implementação

**Arquivos/Classes:**
```
- GraphColoring.h/cpp
  - greedyColoring(Graph, K) → bool
  - selectNodeToSpill() → Node
  - selectWebToSplit() → Web
  
- WebSpilling.h/cpp
  - spillingColoring(Graph, K, maxSpills) → Result
  
- WebSplitting.h/cpp
  - splittingColoring(Graph, K, maxSplits) → Result
  
- CustomAlgorithm.h/cpp
  - customColoring(Graph, K) → Result
```

**Entregas:**
- 4 algoritmos funcionais
- Testes com casos do PDF
- Comparação de resultados (qual usa menos registos?)

---

## 📊 Exemplo Prático - Fluxo Completo

```
INPUT:
ranges.txt:
  sum: 7+,8,9,10-
  i: 1+,2,3,4,5,6-

registers.txt:
  registers: 2
  algorithm: spilling, 1

↓↓↓ PESSOA 2 ↓↓↓
Webs gerados:
  web_0: sum (1 programa point)
  web_1: i (6 programa points)

Interference Graph:
  web_0 -- web_1 (interferem em pontos 7-10? 1-6?)

↓↓↓ PESSOA 3 ↓↓↓
Tentar básico com 2 cores: FALHA (clique tamanho 2 requer 2, mas...)
Tentar spilling com 1 spill: SUCESSO
  - Spill web_1
  - Atribuir web_0 → r0

OUTPUT:
  webs: 2
  web_0: 7+,8,9,10-
  web_1: 1+,2,3,4,5,6-
  registers: 1
  r0: web_0
  M: web_1
```

---

## 🎯 Milestones Comuns (Sincronização)

| Semana | Pessoa 1 | Pessoa 2 | Pessoa 3 |
|--------|----------|----------|----------|
| Sem. 1 | Parser basic | WebBuilder setup | Greedy coloring (T2.1) |
| Sem. 2 | Menu & I/O | Interference graph | Web Spilling (T2.2) |
| Sem. 3 | Output formatter | Testes & debug | Web Splitting (T2.3) |
| Sem. 4 | Doxygen & docs | Integração | Custom algo (T2.4) |
| Sem. 5 | Testing | Demo prep | Demo prep |

---

## 📝 Ficheiros de Teste (do PDF)

**Caso 1 (simples):**
```
sum: 7+,8,9,10-
i: 1+,2,3,4,5,6-
```
Esperado: 2 webs, possível colorir com 2 registos se não interferirem muito

**Caso 2 (overlapping):**
```
y: 7+,8,9,10-
x: 1+,2,3,4,7
x: 7,8
x: 8,9-
x: 8,11,12,13-
```
Esperado: x forma 1 web (ranges sobrepõem), y forma 1 web

---

## ✅ Deliverables Finais (Zip)

```
DA2026_PRJ2_T<TN>_G<GN>.zip
├── Code/
│   ├── src/
│   ├── include/
│   ├── Makefile / CMakeLists.txt
│   └── main executable
├── Documentation/
│   └── html/ (Doxygen output)
└── Presentation.pdf
```

---

## 💡 Dicas

1. **Comunicação:** Usar interfaces claras entre componentes (Pessoa 1 ↔ 2 ↔ 3)
2. **Testing:** Cada pessoa testa seu código com casos simples primeiro
3. **Graph structure:** Baseiem-se na do TP, façam apenas alterações justificadas
4. **Performance:** O(n + m) é esperado - não fazer O(n³)
5. **Demo:** Prepare exemplos visuais (antes/depois graphs, estadísticas)
