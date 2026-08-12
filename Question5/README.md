Question 5 — main.c

What it does
- Demonstrates an interrupt-driven UART frame parser simulated in a single-process program.
- Defines a simple UART frame format with fields: `start_byte`, `src_id`, `dest_id`, `command`, `length`, signed `payload[]`, and `checksum`.
- Computes a XOR checksum over header and payload and validates it on reception.
- Simulates an ISR (`UART_RX_IRQHandler`) that injects bytes into a ring buffer, and uses `node_parse_rx_buffer()` to parse frames.
- `node_send_command()` builds a frame and triggers the RX interrupt byte-by-byte to emulate transmission between nodes.
- Prints success/error messages and payload values to stdout.

Requirements
- C compiler supporting C99 or later (e.g. GCC/MinGW or MSVC).
- Standard C headers: `stdio.h`, `stdint.h`, `stdbool.h`, `string.h`.
- No external libraries required; code is platform independent (pure C).

Build & Run
- Example (MinGW / GCC):

```
cd Question5
gcc -std=c99 -O2 main.c -o main.exe
./main.exe
```

Notes
- The program simulates UART activity in-process; there is no real serial hardware usage.
- The checksum is a simple XOR across header and payload bytes — intended for demonstration, not robust for real-world UART links.
- You can modify `sensor_readings` and node IDs in `main.c` to test different payloads and destinations.
