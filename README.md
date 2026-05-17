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

## Notes
- Interference uses the boundary rule: a web that starts at a definition does not
	interfere with a web that ends at a last-use on the same line.
- Output includes +/- markers for definition and last-use program points.
- For spilling/splitting, the output also includes processing-friendly sections:
  `spills: N` and `splits: N`, listing the web ids involved.