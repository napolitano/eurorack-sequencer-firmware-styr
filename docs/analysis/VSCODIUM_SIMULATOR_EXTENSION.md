<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# VSCodium simulator extension — feasibility analysis

## Goal

Reduce the edit → build → verify cycle without turning VSCodium into a required build system. The CLI build must remain authoritative; editor integration should only orchestrate it.

## Short answer

A Styr extension is feasible, but there are two very different implementations.

### Native launcher extension

A small extension can expose commands such as **Build Simulator**, **Run Simulator**, **Build & Run**, **Run Tests**, and show build state in the status bar. It would call the existing CMake presets and launch the native SDL/OpenGL executable.

Advantages:

- very small extension;
- no simulator rewrite;
- native audio/MIDI behavior remains unchanged;
- useful command/status integration.

Limitation: the SDL window remains an external desktop window. VS Code-family extensions cannot simply dock an arbitrary native HWND/SDL/OpenGL window inside the editor workbench. The turnaround improvement over well-designed tasks is therefore modest.

### Webview + WebAssembly extension

The existing Emscripten target provides a more interesting path. A VSCodium extension can create a WebviewPanel and serve the packaged simulator HTML/JavaScript/WebAssembly/assets through webview-safe URIs. The simulator would then appear as an editor tab.

Potential workflow:

```text
edit src/sequencer
      ↓
CMake/Emscripten incremental build
      ↓
extension refreshes webview bundle
      ↓
Styr simulator remains visible in editor
```

This could materially improve turnaround because the simulator stays next to the source and can eventually expose editor commands, trace loading, screenshots and targeted test actions.

## Main engineering work

1. Make the WebAssembly simulator build reproducible and self-contained.
2. Ensure keyboard, mouse and audio behavior works correctly under a webview content-security policy.
3. Package `.wasm`, JavaScript, data and assets as extension resources.
4. Add a small TypeScript extension host that creates the webview and translates VSCodium commands into simulator actions.
5. Decide whether rebuilds are explicit or triggered by a file watcher. Explicit build/reload is safer initially.
6. Keep MIDI support optional: browser/webview MIDI permissions and native RtMidi behavior are not equivalent.

## Recommendation

Do **not** build the extension yet. First stabilize the native simulator and its tests. The checked-in **Simulator: Build & Run Debug** task should be the baseline workflow. Once the WebAssembly target is green and behaviorally equivalent for the areas we use during development, a webview extension becomes a worthwhile self-contained project.

The extension should never own compiler configuration or source generation. It should consume the same CMake presets that work from a terminal.

From Munich with <img src="../manual/assets/blue-heart.svg" alt="blue heart" width="14">
