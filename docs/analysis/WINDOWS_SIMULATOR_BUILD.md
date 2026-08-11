# Windows Simulator Build

## Step 5.4 — App-local UCRT64 runtime staging

The native Windows build now stages the exact MSYS2 UCRT64 runtime DLLs beside every generated host executable. In particular, the simulator no longer depends on whichever `libstdc++-6.dll` happens to be found through the global Windows `PATH`. This fixes startup failures such as a missing `std::__glibcxx_assert_fail` entry point when a different MinGW runtime is resolved at launch time.

The staged set is `SDL2.dll`, `glew32.dll`, `libstdc++-6.dll`, `libgcc_s_seh-1.dll`, `libwinpthread-1.dll` and `libiconv-2.dll`. The build fails during configuration if any required UCRT64 runtime file is absent.

This also makes launching `styr_simulator.exe` directly from Explorer or VSCodium deterministic; no manual runtime `PATH` prefix is required after a successful build.

## Step 5.3 — UCRT64 host dependencies and SDL entry points

The native Windows simulator uses MSYS2 UCRT64 for GCC, Ninja, SDL2 and GLEW.

Install the complete host dependency set with:

```powershell
C:/msys64/usr/bin/pacman.exe -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-glew
```

Styr resolves SDL2 and GLEW directly below `STYR_UCRT64_PREFIX` (default
`C:/msys64/ucrt64`). This makes the host build independent of whichever CMake
installation happens to be invoked from PowerShell. Missing packages fail at
configure time with the exact MSYS2 installation command.

### SDL2 link boundary

`styr_runtime` is a static library used by both the simulator and the native
test executables. It therefore links only the SDL2 runtime import library.
`SDL2main` and MinGW's `-mwindows` option are deliberately not part of the
runtime's public link interface. Propagating them into the test executables
caused the Windows linker to expect `SDL_main` for every unit/integration test.

Windows host executables instead keep a normal C/C++ `main()` entry point via
`SDL_MAIN_HANDLED`. The simulator and the integration-test runner call
`SDL_SetMainReady()` before the first `SDL_Init()`. This keeps tests as normal
console executables while preserving SDL's required startup state.

### Build

From `src\simulator`:

```powershell
C:/msys64/ucrt64/bin/cmake.exe --preset windows-ucrt64-debug --fresh
C:/msys64/ucrt64/bin/cmake.exe --build --preset windows-ucrt64-debug
C:/msys64/ucrt64/bin/ctest.exe --preset windows-ucrt64-debug
```

From Munich with <img src="../manual/assets/blue-heart.svg" alt="blue heart" width="14">
