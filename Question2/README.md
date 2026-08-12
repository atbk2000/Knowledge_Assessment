Question 2 — main.c

What it does
- Implements a simple button state machine that reacts to the keyboard spacebar:
  - States: `OFF` -> `ON` (when pressed) -> `PROTECTED` (after release)
  - Measures press duration (ms) and prints state transitions and the measured press time
  - Uses a protection timeout during which the button is inert

Requirements
- Windows OS (uses `windows.h` APIs: `GetAsyncKeyState`, `GetTickCount`, `Sleep`).
- C compiler on Windows (MinGW/GCC or MSVC).
- Console application (prints to stdout).

Build & Run
- Example (MinGW / GCC):

```
cd Question2
gcc -std=c99 -O2 main.c -o main.exe -luser32
./main.exe
```

Notes
- The program polls the keyboard in a loop (50 ms tick). Press and release the spacebar to see state transitions.
- `-luser32` may be required when using MinGW to link Windows user32 functions; MSVC typically links required Windows libs automatically.
- Running on non-Windows systems will fail due to missing `windows.h`.
