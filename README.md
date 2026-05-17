# DA_secondProject

## Overview
Register allocation tool using interference graphs and graph coloring. Supports basic, spilling,
splitting, and free (DSATUR-based) variants. Input uses live ranges with +/- markers and produces
web/register assignments as specified in the project description.

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

## Doxygen (T1.3)
The Doxyfile is already configured. Generate HTML docs with:
```bash
doxygen Doxyfile
```
Output goes to Documentation/html/index.html.

## Algorithms

### T2.1 Basic Register Allocation
- Uses greedy simplification + coloring without spills.
- Searches for the minimum number of registers N such that N <= registers from input.

### T2.2 Spilling (heuristic and rationale)
- If coloring fails with the provided register limit, the graph is simplified by spilling webs.
- Spill selection heuristic: pick the web with the highest score
	(score = degree * web_size). This prioritizes high-interference and long webs,
	which tends to reduce congestion quickly.
- We try 0..K spills (K from input), stopping at the first feasible coloring.

### T2.3 Splitting (heuristic and rationale)
- If coloring fails, we split up to K webs to reduce interference.
- Split selection heuristic: same score as spilling (degree * web_size), but only
	webs with size >= 2 are considered.
- Split strategy: split the web by its ordered line list (median cut), creating two
	derived webs. The interference graph is rebuilt after each split.
- This corresponds to inserting save/restore at the split boundary (conceptual),
	as required by the project description.

### T2.4 Free Algorithm
- DSATUR-based heuristic with spills for uncolorable nodes.

## Engineering Decisions & Trade-offs 

### Graph Representation Selection
- We deliberately chose to implement our interference graph using a std::map<int, InterferenceNode> instead of a traditional 2D adjacency matrix.

### Architectural Rationale
- While a 2D matrix provides O(1) edge lookups, real-world compiler interference graphs are highly esparse. Utilizing a map structure dramatically optimizes our RAM overhead by only tracking actual collisions. This trade-off introduces a manageable O(log V) lookup overhead inside the structural map while ensuring scalable memory consumption when compiling massive data sets.

## Quality Assurance & Automated Validation 

### Automated Batch Testing
- To evaluate our software against production-grade workloads, we built a native batch mode execution accessible via the ./main -b argument. This allows our register allocator to seamlessly interface with external automated test benches, processing hundreds of distinct constraint files (ranges.txt) in a few seconds.

### Code Hygiene and Rigor
- Our documentation pipeline is integrated with Doxygen via parameterized custom ALIASES designed to map out clean temporal complexity blocks. The entire source codebase compiles cleanly with zero terminal warnings, ensuring strict design quality.

## Future Work & Architectural Limitations  

### Register Coalescing
- A highly valuable next evolutionary step would be implementing a coalescing phase to catch direct copy assignments (e.g., a = b). If their respective webs do not interfere, they can be fused into the same color, completely removing redundant move instructions from the final compiled binary.

### Dynamic Spill Cost Allocation
- Currently, our spill score selection relies on a static web size and current degree calculation. In a production compiler, instruction lines located inside deeply nested loops or blocks should be weighted exponentially higher, as memory operations within those execution points incur severe runtime performance penalties.

## Notes
- Interference uses the boundary rule: a web that starts at a definition does not
	interfere with a web that ends at a last-use on the same line.
- Output includes +/- markers for definition and last-use program points.
- For spilling/splitting, the output also includes processing-friendly sections:
  `spills: N` and `splits: N`, listing the web ids involved.