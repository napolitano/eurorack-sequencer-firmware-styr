# Simulator third-party code

This directory contains the small simulator dependencies that are intentionally
kept in-tree (`rtmidi` and `stb`). Larger historical dependencies are resolved
by CMake during configure and cached below `build/_deps/`.

The initial Styr migration pins the exact revisions used by the Westlicht
Performer simulator:

| Dependency | Revision |
| --- | --- |
| SoLoud | `1157475881da0d7f76102578255b937c7d4e8f57` |
| NanoVG | `f93799c078fa11ed61c078c65a53914c8782c00b` |
| args | `b7d67237e8bdaa517d7fd6e4e84e1f6efa24f8c5` |
| tinyformat | `aef402d85c1e8f9bf491b72570bfe8938ae26727` |
| pybind11 (optional) | `e250155afadde7100e627e6aa4a541137a863243` |

A local checkout can still be placed at `third_party/simulator/<name>/`; CMake
prefers it over downloading. Set `STYR_SIM_FETCH_DEPENDENCIES=OFF` to enforce a
fully offline build and fail if a local dependency is missing.
