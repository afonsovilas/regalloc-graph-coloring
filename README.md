# Register Allocator (Graph Coloring)
 
Register allocation tool in C++ based on interference graphs and graph coloring. It supports four variants: basic allocation, spilling, splitting, and a free DSATUR-based heuristic. The input describes variable live ranges with `+`/`-` markers, and the output assigns each web to a register.
 
Developed as the second project for the **Algorithm Design (Desenho de Algoritmos)** course of the Bachelor in Informatics and Computing Engineering at the Faculty of Engineering of the University of Porto (FEUP), 2025/2026.
 
**Team:** [Name 1], [Name 2], [Name 3], Afonso Vilas
 
## Build
 
```bash
make
```
 
## Run
 
Interactive menu:
 
```bash
./main
```
 
Batch mode:
 
```bash
./main -b ranges.txt registers.txt allocation.txt
```
 
## Documentation
 
The project is documented with Doxygen. To generate the HTML documentation:
 
```bash
doxygen Doxyfile
```
 
The output is written to `Documentation/html/index.html`.
 
## Algorithms
 
### Basic register allocation
- Greedy simplification followed by coloring, without spills.
- Finds the minimum number of registers N needed, as long as N does not exceed the number of registers given in the input.
### Spilling
- If the graph cannot be colored with the available registers, webs are spilled to simplify it.
- **Heuristic:** spill the web with the highest score, where `score = degree × web size`. This favours long webs with many conflicts, which usually reduces interference fastest.
- The tool tries 0 to K spills (K given in the input) and stops at the first valid coloring.
### Splitting
- If coloring fails, up to K webs are split to reduce interference.
- **Heuristic:** the same score as spilling (`degree × web size`), considering only webs with at least two lines.
- **Strategy:** each selected web is split at the median of its ordered line list, creating two new webs. The interference graph is rebuilt after each split.
- Conceptually, this corresponds to inserting save/restore instructions at the split point.
### Free algorithm
- DSATUR-based coloring heuristic, spilling the nodes that cannot be colored.
## Design decisions
 
### Graph representation
The interference graph is stored as a `std::map<int, InterferenceNode>` rather than a 2D adjacency matrix. Interference graphs are usually sparse, so storing only the existing edges uses much less memory. The trade-off is an O(log V) lookup instead of the O(1) access of a matrix.
 
### Batch mode
The `-b` option runs the allocator directly on input files, without the interactive menu. This makes it easy to test the tool automatically on many input files.
 
### Code quality
The code is documented with Doxygen, including the time complexity of the main functions, and compiles without warnings.
 
## Limitations and future work
 
- **Register coalescing:** a coalescing phase could merge webs connected by copy instructions (e.g. `a = b`) when they do not interfere, assigning them the same register and removing unnecessary move instructions.
- **Spill cost:** the current spill score uses only web size and degree. A more realistic cost model would give higher weight to lines inside loops, where memory accesses are more expensive.
## Notes
 
- Interference follows the boundary rule: a web that starts at a definition does not interfere with a web that ends at a last use on the same line.
- The output includes `+`/`-` markers for definition and last-use points.
- For spilling and splitting, the output also includes `spills: N` and `splits: N` sections listing the affected web ids.
