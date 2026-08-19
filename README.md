# Atari 2600 Emulator

A cycle-accurate Atari 2600 emulator written in C++, implementing the full MOS 6502 CPU instruction set, the TIA (Television Interface Adaptor) video chip, and the RIOT timer, rendered live via SDL2.

## Features

**6502 CPU** — all documented instructions across every addressing mode (immediate, zero page, zero page X/Y, absolute, absolute X/Y, indirect, indexed indirect, indirect indexed, relative, implied/accumulator), dispatched through a 16x16 opcode table with matching cycle-cost and addressing-mode lookup tables, including the +1-cycle page-crossing penalties real 6502 hardware incurs.

**TIA (video chip)** — cycle-accurate beam position tracking (horizontal/vertical position advances every TIA cycle, 3 TIA cycles per CPU cycle, matching real hardware timing), full playfield rendering (with mirror/repeat modes), both player sprites with every NUSIZ stretch/repeat/multi-copy mode, both missiles, the ball, horizontal motion (HMOVE) fine positioning, and hardware collision detection (player-missile, player-playfield, missile-ball, etc., matching the real CXxx collision registers) with correct color-priority compositing.

**RIOT timer** — the interval timer used for game logic timing, implemented separately from the CPU/TIA loop.

**Input and display** — SDL2 window with keyboard-mapped joystick input (arrow keys and WASD for two-player local testing) and console switches (reset/select), rendering the TIA's 160x192 visible frame scaled into a resizable window.

**Debug tooling** — optional per-instruction trace logging, a hex memory-grid dump, and frame counters, all toggle-able via flags on the CPU state struct.

## Structure

```
src/
  cpu.h / cpu.cpp        6502 registers, opcode dispatch table, instruction implementations
  tia.h / tia.cpp        Beam timing, sprite/playfield rendering, collision detection
  timer.h / timer.cpp    RIOT interval timer
  debug.h / debug.cpp    Instruction disassembly strings and trace helpers
  main.cpp               SDL2 window/input setup, ROM loading, and the main emulation loop
  cpuTest.cpp            Standalone CPU unit tests
  Combat.A26              Sample ROM used for testing
pdfs/
  Reference documentation used during development (6502 reference, Atari 2600 TIA technical manual)
```

## Building and running

Requires SDL2 (via `pkg-config`) and a C++17 compiler.

```bash
cd src
make
./Atari2600 Combat.A26
```

`make cpuTest` builds the standalone CPU instruction test suite.

## Controls

Arrow keys / WASD move the two joysticks, Z / Left-Ctrl and E fire, 1 and 2 are the Reset and Select console switches, Space single-steps a frame when frame-stepping is enabled, Escape quits.

## Notes for whoever's reading this on GitHub

- `input.cpp`, `memory.cpp`, `rom loader.cpp`, and `ui.cpp` are present but empty — input handling, ROM loading, and windowing are all implemented directly in `main.cpp` rather than split out, so those files aren't part of the current build.
- Development notes comparing frame/scanline timing against a reference emulator, plus a running TODO list, are kept separately rather than in this README — worth trimming if you want a purely user-facing doc.
