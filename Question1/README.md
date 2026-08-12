Question 1 — main.c

What it does
- Parses a statically-provided integer array in `main.c` and computes basic statistics:
  - Maximum value
  - Minimum value
  - Average (floating point)
  - Collects all even values into a dynamically allocated array
- Prints the computed statistics to stdout and frees allocated memory before exit.

Requirements
- C compiler supporting C99 or later (e.g. GCC/MinGW or MSVC).
- Standard C library (`stdio.h`, `stdlib.h`) only.

Build & Run
- Example (MinGW / GCC):

```
cd Question1
gcc -std=c99 -O2 main.c -o main.exe
./main.exe
```

Notes
- `main.c` allocates memory for even values using `malloc`/`realloc`; the program frees that memory on exit.
- No special platform dependencies.
