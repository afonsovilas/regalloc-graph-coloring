# ROADMAP EXECUTIVO - 3 Pessoas / 20 Pontos / 5 Semanas

## 🎯 Objetivo
Implementar ferramenta de **Compiler Register Allocation** usando **Graph Coloring**.

**Deadline:** 17 Maio 2026, 23:59 (PT)  
**Grupo:** 3 pessoas  
**Linguagem:** C++, Java ou Python

---

## 👤 PESSOA 1: Infrastructure & Coordination (4 pontos)

### Responsabilidades
- **T1.1 (1 pt):** Menu interativo + Batch mode `-b ranges.txt registers.txt output.txt`
- **T1.2 (1 pt):** Parser de ficheiros + estruturas de dados
- **T1.3 (2 pts):** Documentação Doxygen + análise complexidade
- **Plus:** Output formatter (escrever ficheiro resultado)

### Entregáveis
```
src/
  ├── main.cpp (menu + batch)
  ├── InputParser.h/cpp
  ├── OutputFormatter.h/cpp
  ├── Graph.h/cpp (base, adaptar do TP)
  └── DataStructures.h (tipos compartilhados)
```

### Testes
- [ ] Parse ranges.txt corretamente
- [ ] Parse registers.txt corretamente
- [ ] Batch mode recebe argumentos certos
- [ ] Menu interativo funciona
- [ ] Output.txt tem formato certo

---

## 👤 PESSOA 2: Web & Interference Graph

### Responsabilidades
- **Web Building:** Merge de live ranges overlapping
  - Input: Ranges de mesma variável
  - Output: Webs únicos (e.g., `i: 1-6` + `i: 9-12` → 2 webs)
  
- **Interference Graph:** Construir grafo de interferência
  - Nodes: Webs (web_0, web_1, ...)
  - Edges: Webs que coexistem em algum programa point
  - **Cuidado:** Semântica correta (def/use na mesma linha)

### Entregáveis
```
src/
  ├── WebBuilder.h/cpp
  │   ├── mergeRanges()
  │   └── buildWebs()
  └── InterferenceGraph.h/cpp
      ├── buildGraph()
      └── isInterfering()
```

### Testes
- [ ] Merge ranges simples (sem overlap)
- [ ] Merge ranges com overlap
- [ ] Merge ranges contíguos (fusão)
- [ ] Construir graph com 2-3 webs
- [ ] Validar edges corretos
- [ ] Testar com casos do PDF (Figure 5, 6)

---

## 👤 PESSOA 3: Graph Coloring & Algorithms (12 pontos)

### T2.1 - Basic Coloring (4 pts)
**Algoritmo:** Greedy graph coloring (Figure 9 do PDF)

```
1. Clique Removal: Remove nós com grau < K até saturação
2. Spilling: Se todos têm grau ≥ K, spill um nó (push stack)
3. Coloring: Pop stack, atribuir cor ≠ vizinhos
```

**Complexidade:** O(n + m)  
**Testes:**
- [ ] Testar com K=2,3,4 em graphs simples
- [ ] Validar atribuição válida (sem conflitos)
- [ ] Retornar true/false corretamente

---

### T2.2 - Web Spilling (3 pts)
**Ideia:** Se básico falha com K registos, spill até M webs (deixar em memory)

**Strategy:** Escolher qual web spill
- Opção 1: Maior grau
- Opção 2: Menor tamanho
- **Documentar escolha!**

**Algoritmo:**
```
1. Tenta greedyColoring com K
2. Se falha: remove web com score mais alto
3. Repete até sucesso ou esgota limite M
```

**Testes:**
- [ ] Testar com M=1,2,3
- [ ] Validar webs spilled estão em output (M:)
- [ ] Verificar atribuição válida

---

### T2.3 - Web Splitting (3 pts)
**Ideia:** Se básico falha, particionar webs em sub-webs

**Strategy:** Como splittar?
- Opção 1: Pares vs ímpares (linha % 2)
- Opção 2: Por ponto de maior interferência
- **Documentar escolha!**

**Algoritmo:**
```
1. Tenta greedyColoring
2. Se falha: split web com score mais alto
3. Reconstrói graph com sub-webs
4. Repete até sucesso ou esgota limite M
```

**Testes:**
- [ ] Testar com M=1,2,3
- [ ] Validar sub-webs têm linhas corretas
- [ ] Verificar graph reconstruído corretamente

---

### T2.4 - Custom Algorithm (4 pts)
**Liberdade total!** Sugestões:

- Coloring heurístico melhorado (best-fit em vez de first-fit)
- Combinar spilling + splitting dinamicamente
- Live range coalescing (merge webs compatíveis)
- Usar métricas de grafo (planarity, articulation points)

**Requisito:** Documentar a ideia e comparar com básico

**Testes:**
- [ ] Algoritmo funciona em todos casos básicos
- [ ] Comparação com T2.1: qual usa menos registos?
- [ ] Casos onde custom ganha do básico

---

## 📊 Fluxo Completo (Exemplo)

```
ranges.txt:
  sum: 1+,2,3-
  i: 2+,3,4,5,6-

registers.txt:
  registers: 2
  algorithm: basic

↓ (Pessoa 1: Parse)
InputData {
  allVariables: {sum: [1+,2,3-], i: [2+,3,4,5,6-]},
  numRegisters: 2,
  algorithm: "basic"
}

↓ (Pessoa 2: Webs + Graph)
WebList {
  web_0: sum {1,2,3}
  web_1: i {2,3,4,5,6}
}
InterferenceGraph {
  nodes: 2
  edges: (0-1) [interferem linha 2,3]
}

↓ (Pessoa 3: Coloring)
greedyColoring(graph, K=2) {
  Max degree = 1
  Colores: web_0 → color 0, web_1 → color 1
  ✅ Success!
}

↓ (Pessoa 1: Output)
output.txt:
  webs: 2
  web0: 1+,2,3-
  web1: 2+,3,4,5,6-
  registers: 2
  r0: web0
  r1: web1
```

---

## 📅 Timeline Recomendado

| Semana | Fase | Pessoa 1 | Pessoa 2 | Pessoa 3 |
|--------|------|----------|----------|----------|
| 1 (May 1-5) | Design | Plan interface | Plan logic | Learn algorithm |
| 2 (May 6-10) | Alpha | Parser + Menu | WebBuilder | T2.1 basic |
| 3 (May 11-13) | Beta | I/O integration | InterferenceGraph | T2.1 tested |
| 4 (May 14-16) | Final | Doxygen docs | Graph validated | T2.2/T2.3/T2.4 |
| 5 (May 17) | **DEADLINE** | Review | Final tests | Demo ready |

---

## ✅ Deliverables (Zip: `DA2026_PRJ2_T<TN>_G<GN>.zip`)

```
DA2026_PRJ2_T01_G03.zip
├── Code/
│   ├── src/ (*.cpp)
│   ├── include/ (*.h)
│   ├── Makefile / CMakeLists.txt
│   ├── test/ (optional)
│   └── executable (myProg ou myProg.exe)
│
├── Documentation/
│   └── html/ (Doxygen generated)
│
└── Presentation.pdf (PowerPoint convertido)
```

---

## 🔑 Key Concepts Checklist

- [ ] **Live Range:** Linhas onde variável pode ser usada
- [ ] **Web:** Merge de live ranges overlapping
- [ ] **Interference:** Dois webs coexistem num ponto
- [ ] **Graph Coloring:** Assign cores (registos) sem conflito
- [ ] **Clique:** Subgrafo completo (todos conectados)
- [ ] **Spilling:** Web → Memory (M) se não cabe
- [ ] **Splitting:** Partir web em sub-webs
- [ ] **Degree:** Número de vizinhos dum nó

---

## 🚀 Success Criteria

✅ **FUNCIONAL:**
- Batch mode executa sem erro
- Menu interativo funciona
- Parse ficheiros corretos
- Output formato certo
- Todos 4 algoritmos (T2.1-T2.4) funcionam

✅ **CORRETO:**
- Graph coloring retorna atribuição válida
- Nenhum conflito de cores
- Webs com output correto
- Interference graph correto

✅ **DOCUMENTADO:**
- Doxygen comments em todas classes
- Complexidade O(...) documentada
- README com instruções
- Presentation.pdf com 7-8 slides

✅ **TESTADO:**
- Rodar com casos PDF (Figure 7, 8)
- Verificar output com valores esperados
- Sem memory leaks
- Sem crashes edge-cases

---

## 💡 Tips & Tricks

1. **Comunicação:** Use Discord/Slack + reunião sexta
2. **Git:** Branches pessoa1/, pessoa2/, pessoa3/
3. **Testing:** Cada pessoa testa seu módulo isolado
4. **Integration:** Reunir 3x semana para testar junto
5. **Demo:** Prepare 3-4 examples interessantes
6. **Questions:** Se dúvida, pergunte ao professor

---

## 📞 Support

- Dúvidas arquivo PDF? Reler secção 2, 3, Appendix A
- Dúvidas algoritmo? Rever Figure 9 do PDF
- Dúvidas output? Rever Figure 11, 12
- Dúvidas input? Rever Figure 7, 8

---

## 🎓 Grading (Total 20 pts)

| Task | Points | Criteria |
|------|--------|----------|
| T1.1 | 1 | Menu + batch mode work |
| T1.2 | 1 | Parser correct |
| T1.3 | 2 | Doxygen + complexity |
| T2.1 | 4 | Basic coloring works |
| T2.2 | 3 | Spilling works |
| T2.3 | 3 | Splitting works |
| T2.4 | 4 | Custom algo works |
| T3.1 | 2 | Demo + presentation |
| **TOTAL** | **20** | |

---

## 🔔 IMPORTANTE

> ⚠️ **Deadline:** Sexta 17 Maio, 23:59 PT
>
> ⚠️ **Late:** -10% (24h), -25% (48h), depois reject
>
> ⚠️ **Format:** ZIP exacto: `DA2026_PRJ2_T<TN>_G<GN>.zip`
>
> ⚠️ **Graph:** OBRIGATÓRIO usar estrutura Graph do TP

---

## 📋 Pre-Submission Checklist (May 16)

- [ ] Código compila sem erros/warnings
- [ ] Batch mode: `./myProg -b test1.txt test_regs.txt out.txt` funciona
- [ ] Menu interativo: todas opções funcionam
- [ ] Output ficheiro tem formato correto
- [ ] Doxygen HTML gerado
- [ ] Sem memory leaks (valgrind)
- [ ] Git history bem organizado
- [ ] README com instruções compilação
- [ ] Presentation.pdf com 7-8 slides
- [ ] ZIP criado com estrutura correcta

**DONE? → Submit para Moodle!** ✅

