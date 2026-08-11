# Shared support

This directory is intentionally small. It contains generic Styr facilities used
by more than one target, currently low-level `core` utilities and the common OS
lock helper.

Sequencer product logic does **not** live here. Engines, models, UI, generators,
configuration and application state belong to `src/sequencer/`, even though the
simulator compiles and executes them as part of its host build.
