# Third-party source

Everything below this directory is imported code or a cache of pinned imported code. It is not Styr-native source merely because it is built with Styr.

- `common/` — dependencies used by shared runtime code.
- `embedded/` — dependencies used only by the STM32 firmware or bootloader.
- `simulator/` — dependencies used only by the host simulator.

Each dependency retains its own license and provenance. Modernization should replace or update dependencies independently rather than mixing dependency changes with unrelated feature work.
