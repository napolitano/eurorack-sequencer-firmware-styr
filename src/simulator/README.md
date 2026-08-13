# Simulator

The Styr simulator is a desktop development host for the real sequencer implementation under `src/sequencer/`. It is intended to shorten feature-development and debugging cycles; it is not a separate sequencer codebase.

## Dependencies

The normal simulator build requires:

- CMake 3.20 or newer;
- Ninja;
- a native C/C++ compiler;
- SDL2 development files;
- OpenGL development files;
- GLEW on native Windows builds.

Historical source dependencies (SoLoud, NanoVG, args and tinyformat) are fetched automatically by CMake at the exact revisions used by PER|FORMER and cached below `build/_deps/`. If a matching local checkout exists under `third_party/simulator/<dependency>/`, CMake uses it instead.
The first configure therefore needs Git and network access unless those dependencies are already cached or provided locally. Subsequent configures reuse the cache.

Python bindings are optional and disabled by default. Enable them explicitly with `-DSTYR_SIM_ENABLE_PYTHON=ON`; doing so also resolves the pinned pybind11 dependency.

For an offline configure, pre-populate the local dependency directories and configure with `-DSTYR_SIM_FETCH_DEPENDENCIES=OFF`.

## Windows — MSYS2 UCRT64

Install the native host prerequisites from PowerShell or an **MSYS2 UCRT64** shell. From PowerShell the complete command is:

```powershell
C:/msys64/usr/bin/pacman.exe -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-glew
```

Configure and build from PowerShell or VSCodium:

```powershell
cd src\simulator
C:\msys64\ucrt64\bin\cmake.exe --preset windows-ucrt64-debug
C:\msys64\ucrt64\bin\cmake.exe --build --preset windows-ucrt64-debug
```

Run the simulator from the repository root:

```powershell
.\build\simulator\windows-ucrt64-debug\styr_simulator.exe
```

The Windows build stages the matching UCRT64 runtime DLLs (`SDL2.dll`, `glew32.dll`, `libstdc++-6.dll`, `libgcc_s_seh-1.dll`, `libwinpthread-1.dll` and `libiconv-2.dll`) beside each generated host executable. This keeps simulator and test startup independent of the caller's `PATH` and avoids mixing GCC runtimes from unrelated Windows toolchains.

Run the simulator-specific CTest suite:

```powershell
cd src\simulator
C:\msys64\ucrt64\bin\ctest.exe --preset windows-ucrt64-debug
```

Product-code unit tests are not owned by this CMake project; run them from the repository root with `pio test -e test_bootloader_native` and `pio test -e test_product_native`. The Windows preset explicitly selects UCRT64 GCC/G++, Ninja and the UCRT64 prefix so STM32CubeCLT or an unrelated `cc.exe` in the normal Windows `PATH` cannot silently become the simulator toolchain.

SDL2 and GLEW are resolved explicitly from `C:/msys64/ucrt64` instead of through the invoking CMake installation's helper modules. The runtime links only the SDL2 DLL import library; `SDL2main` and `-mwindows` are intentionally excluded so native test executables keep normal `main()` entry points.

## Linux / macOS

```bash
cd src/simulator
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
../../build/simulator/debug/styr_simulator
```

## Documentation screenshots

The simulator also provides a headless documentation-capture target. It drives the real sequencer UI without opening a desktop window and writes deterministic LCD framebuffer PNGs directly to `docs/manual/assets/`.

The default documentation scale is 3×, so the native 256×64 LCD becomes 768×192 pixels. Scaling uses integer nearest-neighbour pixel replication only; no filtering or anti-aliasing is applied.

After configuring the simulator, regenerate the complete manual screenshot set with:

```powershell
C:\msys64\ucrt64\bin\cmake.exe --build --preset windows-ucrt64-debug --target manual-screenshots
```

On Linux/macOS use the corresponding native preset. The scale can be changed during configure with `-DSTYR_DOC_SCREENSHOT_SCALE=N`, where `N` is an integer from 1 through 8. See [`docs/development/documentation/README.md`](../../docs/development/documentation/README.md).

## VSCodium

The checked-in tasks provide the shortest native loop:

1. run **Simulator: Build & Run Debug**;
2. make a source change under `src/sequencer/`;
3. run the same task again.

CMake/Ninja recompiles only affected translation units. **Simulator: Test Debug** runs the test suite without changing the embedded build.


## WebAssembly

With an activated Emscripten SDK (`EMSDK` set):

```bash
cmake --preset web
cmake --build --preset web
./scripts/package_web.sh
```

The WebAssembly target is also the most promising technical basis for an eventual in-editor simulator because a VSCodium/VS Code webview can host web content directly, whereas a native SDL/OpenGL window cannot be embedded cleanly into the editor workbench.
