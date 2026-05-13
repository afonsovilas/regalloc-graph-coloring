# 📚 Projeto Register Allocation - Documentação Completa

## 🎯 Objetivo do Projeto
Implementar ferramenta de **alocação de registos em compiladores** usando **graph coloring** (problema NP-complete).

**Deadline:** 17 Maio 2026, 23:59 PT  
**Grupo:** 3 pessoas (3 é o recomendado)  
**Pontos totais:** 20 pontos

---

## 📖 Documentos Principais

### 1️⃣ **EXECUTIVE_ROADMAP.md** ⭐ **COMECE AQUI**
**O que é:** Resumo executivo de uma página com tudo que precisa saber.

**Contém:**
- Objetivo + deadline
- Responsabilidades de cada pessoa (super conciso)
- Fluxo completo com exemplo
- Timeline 5 semanas
- Success criteria
- Checklist final

**Público:** Todos os 3 elementos do grupo


### 2️⃣ **QUICK_SUMMARY.md**
**O que é:** Visual overview com diagramas e tabelas.

**Contém:**
- Diagrama fluxo dados
- Checklist por pessoa
- Timeline detalhada
- Exemplo end-to-end
- Conceitos críticos
- Tips & tricks

**Público:** Todos (especialmente para referência rápida)


### 3️⃣ **PROJECT_DIVISION.md**
**O que é:** Divisão detalhada das 3 partes com toda implementação.

**Contém:**
- Responsabilidades detalhadas por pessoa
- Tarefas T1.1-T2.4 com descrição completa
- Entregáveis específicos (ficheiros, classes)
- Exemplos e pseudocódigo
- Milestones comuns
- Ficheiros de teste

**Público:** Cada pessoa (lê sua secção + comuns)


### 4️⃣ **INTERFACES_AND_DEPENDENCIES.md**
**O que é:** Especificação técnica de interfaces entre componentes.

**Contém:**
- Estrutura de módulos (diagrama)
- Interfaces principais (4 principais)
- Header files compartilhados
- Fluxo de comunicação
- Checklist integração
- Common integration issues
- Validação de interface

**Público:** Developers (para implementação técnica)


### 5️⃣ **PDF Original Anexado**
**O que é:** Enunciado oficial do projeto.

**Contém:**
- Descrição detalhada problema
- 3-address instructions (Appendix A)
- Control-flow graphs
- Live ranges & webs
- Interference graphs
- Algoritmos propostos
- Input/output formato

**Público:** Referência sempre que em dúvida


---

## 🚀 Como Começar (Para o Grupo)

### Passo 1: Leitura Inicial (30 min)
1. **Pessoa 1:** Lê [EXECUTIVE_ROADMAP.md](EXECUTIVE_ROADMAP.md) + secção dela em [PROJECT_DIVISION.md](PROJECT_DIVISION.md)
2. **Pessoa 2:** Lê [EXECUTIVE_ROADMAP.md](EXECUTIVE_ROADMAP.md) + secção dela em [PROJECT_DIVISION.md](PROJECT_DIVISION.md)
3. **Pessoa 3:** Lê [EXECUTIVE_ROADMAP.md](EXECUTIVE_ROADMAP.md) + secção dela em [PROJECT_DIVISION.md](PROJECT_DIVISION.md)

### Passo 2: Reunião de Design (1 hora)
- Discutir [INTERFACES_AND_DEPENDENCIES.md](INTERFACES_AND_DEPENDENCIES.md)
- Acordar em DataStructures.h (tipos compartilhados)
- Planejar timeline
- Setup Git branches

### Passo 3: Implementação
- Cada pessoa trabalha sua parte (ver [PROJECT_DIVISION.md](PROJECT_DIVISION.md))
- Reuniões 2-3x semana para validar interfaces
- Use [QUICK_SUMMARY.md](QUICK_SUMMARY.md) para referência rápida

### Passo 4: Integração (Semana 3-4)
- Validar interfaces em [INTERFACES_AND_DEPENDENCIES.md](INTERFACES_AND_DEPENDENCIES.md)
- Rodar testes end-to-end
- Corrigir bugs integração

### Passo 5: Demo & Presentation (Semana 5)
- Preparar 3-4 casos de teste
- Gravação ou apresentação ao vivo (10 min)
- Adicionar slides PowerPoint

---

## 👥 Mapa Rápido por Pessoa

### 👤 Pessoa 1: Infrastructure (4 pts)
**Ficheiro:** [PROJECT_DIVISION.md#pessoa-1](PROJECT_DIVISION.md)

**Tarefas:**
- T1.1 (1 pt): Menu interativo + batch mode
- T1.2 (1 pt): Parser ficheiros
- T1.3 (2 pts): Doxygen + complexidade

**Responsável por:** Coordenação, integração, I/O


### 👤 Pessoa 2: Data Structures
**Ficheiro:** [PROJECT_DIVISION.md#pessoa-2](PROJECT_DIVISION.md)

**Tarefas:**
- Web building (merge ranges)
- Interference graph construction
- Algoritmo detecção interferência

**Responsável por:** Estruturas de dados, parsing lógico


### 👤 Pessoa 3: Algorithms (12 pts)
**Ficheiro:** [PROJECT_DIVISION.md#pessoa-3](PROJECT_DIVISION.md)

**Tarefas:**
- T2.1 (4 pts): Basic graph coloring
- T2.2 (3 pts): Web spilling
- T2.3 (3 pts): Web splitting
- T2.4 (4 pts): Custom algorithm

**Responsável por:** Algoritmos núcleo, alocação registos


---

## 📊 Structure Geral do Projeto

```
├── EXECUTIVE_ROADMAP.md ⭐ (LEIA PRIMEIRO!)
├── QUICK_SUMMARY.md
├── PROJECT_DIVISION.md (detalhado)
├── INTERFACES_AND_DEPENDENCIES.md (técnico)
└── [PDF Original do Enunciado]

Code Structure:
├── src/
│   ├── main.cpp (Pessoa 1)
│   ├── InputParser.cpp (Pessoa 1)
│   ├── OutputFormatter.cpp (Pessoa 1)
│   ├── WebBuilder.cpp (Pessoa 2)
│   ├── InterferenceGraph.cpp (Pessoa 2)
│   ├── Graph.cpp (base, shared)
│   └── GraphColoring.cpp (Pessoa 3)
│       ├── greedyColoring (T2.1)
│       ├── spillingColoring (T2.2)
│       ├── splittingColoring (T2.3)
│       └── customColoring (T2.4)
├── include/
│   ├── DataStructures.h (COMPARTILHADO!)
│   ├── Graph.h
│   └── (outros headers)
└── test/
    └── test_cases.txt
```

---

## 🔑 Key Concepts (Quick Reference)

| Conceito | Explicação | Implementado por |
|----------|-----------|---|
| **Live Range** | Linhas programa onde variável está viva | Pessoa 1 parse |
| **Web** | Merge de live ranges que se sobrepõem | Pessoa 2 |
| **Interference** | Dois webs coexistem num programa point | Pessoa 2 |
| **Interference Graph** | Grafo: nodes=webs, edges=interferência | Pessoa 2 |
| **Graph Coloring** | Atribuir cores (registos) sem conflito | Pessoa 3 |
| **K registos** | Número máximo cores disponíveis | Input |
| **Spilling** | Deixar web em memory (M) se não cabe em K | Pessoa 3 (T2.2) |
| **Splitting** | Partir web em sub-webs para melhor coloring | Pessoa 3 (T2.3) |

---

## ⏰ Timeline (5 Semanas)

| Semana | Milestone | Pessoa 1 | Pessoa 2 | Pessoa 3 |
|--------|-----------|----------|----------|----------|
| **1** | Setup | Parser alpha | WebBuilder design | Learn algo |
| **2** | Alpha | Parser + Menu | WebBuilder code | T2.1 basic |
| **3** | Beta | I/O integration | InterferenceGraph | T2.1 tested |
| **4** | Final | Doxygen docs | Validation | T2.2/T2.3/T2.4 |
| **5** | 🎯 DEADLINE | Review | Final bugs | Demo ready |

---

## ✅ Validation Checklist

### Antes de Integrar (cada pessoa)
- [ ] Código compila sem erros
- [ ] Funciona em casos simples
- [ ] Testes unitários passam
- [ ] Memory leaks checked

### Antes de Submeter (grupo todo)
- [ ] Batch mode funciona
- [ ] Menu interativo funciona
- [ ] Output ficheiro correto
- [ ] Doxygen HTML gerado
- [ ] Presentation.pdf pronto
- [ ] ZIP estrutura correcta
- [ ] README com instruções

---

## 📞 FAQ Rápido

**P: Por onde começamos?**  
R: Leia [EXECUTIVE_ROADMAP.md](EXECUTIVE_ROADMAP.md) em 10 min. Depois reunião para acordar DataStructures.h.

**P: Que linguagem usar?**  
R: C++ (recomendado), Java ou Python. Tem de compilar/rodar.

**P: Preciso de compiler teórico?**  
R: Não! Apenas alocação de registos. Já temos 3-address instructions.

**P: Qual é o algoritmo hard?**  
R: T2.1 (graph coloring). T2.2/T2.3 são iterativas do mesmo.

**P: Podemos usar bibliotecas?**  
R: Sim (Boost, STL, etc). Graph tem de ser basicamente do TP.

**P: Como teste?**  
R: Use casos simples (Figure 7, 8 do PDF). Depois cases complexas.

**P: E se não conseguirmos colorir?**  
R: Output ficheiro com registers: 0 e todos webs em memory (M:).

---

## 🎯 Success Metrics

**MÍNIMO PASSÁVEL (60% / ~12 pts):**
- T1.1, T1.2, T1.3 todos funcionam (4 pts)
- T2.1 (basic coloring) funciona bem (4 pts)
- Output correto (2 pts) 
- ➜ Mas falha T2.2/T2.3/T2.4

**BOM (80% / ~16 pts):**
- Tudo acima MAIS
- T2.2 (spilling) funciona
- T2.3 (splitting) funciona
- Documentação boa

**EXCELENTE (100% / 20 pts):**
- Tudo acima MAIS
- T2.4 (custom algoritmo) criativo e funciona bem
- Documentação excelente
- Presentation impactante

---

## 📋 Ficheiros Principais a Criar

### Ficheiros de Código (Pessoa 1)
```cpp
main.cpp              // main() + menu interativo
InputParser.h/cpp     // parse ranges.txt, registers.txt
OutputFormatter.h/cpp // write output.txt
DataStructures.h      // tipos compartilhados
```

### Ficheiros de Código (Pessoa 2)
```cpp
WebBuilder.h/cpp           // buildWebs(), mergeRanges()
InterferenceGraph.h/cpp    // buildGraph(), isInterfering()
```

### Ficheiros de Código (Pessoa 3)
```cpp
GraphColoring.h/cpp  // greedyColoring()
WebSpilling.h/cpp    // spillingColoring()
WebSplitting.h/cpp   // splittingColoring()
CustomAlgo.h/cpp     // customColoring()
```

### Build Files
```
Makefile              // ou CMakeLists.txt
README.md             // instruções compilação
```

### Documentation
```
Documentation/html/   // Doxygen output
Presentation.pdf      // PowerPoint convertido
```

---

## 🚀 Próximos Passos

1. **Distribuir este documento** a todos 3 elementos
2. **Reunião Setup (1 hora):**
   - Escolher linguagem (C++ recomendado)
   - Acordar DataStructures.h
   - Setup Git
   - Distribuir tarefas exatas
3. **Começar implementação Semana 1:**
   - Pessoa 1: Setup projeto + parser básico
   - Pessoa 2: WebBuilder inicial
   - Pessoa 3: Estude algoritmo + comece T2.1
4. **Reuniões semanais:** Sexta 3PM para sincronização

---

## 📚 Referências

- **Enunciado oficial:** PDF anexado (ver documento original)
- **Algoritmo greedy coloring:** Figure 9 do PDF
- **Exemplos input/output:** Figure 7, 8, 11, 12 do PDF
- **3-address instructions:** Appendix A do PDF
- **Interference graph:** Section 2.5 do PDF

---

## ✨ Boas Práticas

- **Git:** Use branches (`pessoa1/`, `pessoa2/`, `pessoa3/`)
- **Communication:** Discord/Slack + reuniões semanais
- **Testing:** Cada pessoa testa seu módulo isoladamente
- **Integration:** Reunir 2-3x semana para testar junto
- **Documentation:** Doxygen comments enquanto código
- **Demo:** Prepare exemplos visuais interessantes

---

**BOA SORTE! 🎓**

*Qualquer dúvida, pergunte ao professor ou consulte o PDF do enunciado.*

