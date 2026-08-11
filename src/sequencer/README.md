# Sequencer

This is the Styr product implementation.

- `engine/` — sequencing, timing, routing and generators
- `model/` — projects, tracks, sequences, persistence and settings
- `ui/` — pages, controllers and painters
- `platform/stm32/` — hardware-specific implementation for the real device
- `main.cpp` — embedded application entry point

The simulator is a host for this code. It compiles the same sequencer sources
directly from this directory instead of maintaining a separate copy.
