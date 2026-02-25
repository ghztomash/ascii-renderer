# asciiRenderer

## Disclaimer

This repository is shared for reference and personal use.  
It is **not expected to run as-is** on another machine without local setup, matching openFrameworks/addons versions, and environment-specific assets/configuration.

## Overview

`asciiRenderer` is an openFrameworks app that renders animated scenes into ASCII output.  
It combines multiple renderer modules (including Lua-driven renderers), applies character/color mapping, and supports recording/export workflows.

## Main Features

- Renderer stack architecture (`BaseRenderer`, factory-based renderer creation).
- Lua renderer with script hot-reload and runtime error handling.
- Character set + font based ASCII conversion.
- Overlay/grid/flip effects and theme support.
- Frame capture and recording helpers.

## Repository Layout

- `src/`: app source, renderers, Lua bindings interface/wrappers.
- `bin/data/`: runtime assets and data (`scripts/`, `fonts/`, `shaders/`, `projects/`, `configs/`).

## Dependencies

This project depends on openFrameworks and these addons (see `addons.make`):

- `ofxGui`
- `ofxXmlSettings`
- `ofxFontStash`
- `ofxTimeMeasurements`
- `ofxWaveforms`
- `ofxEosParticles`
- `ofxLua`

## Build and Run

From the project root:

```bash
make Debug
make RunDebug
```

Release build:

```bash
make Release -j$(nproc)
./bin/asciiRenderer
```

If openFrameworks root is not in the default location, set `OF_ROOT` in `config.make`.

## Lua Workflow

- Lua scripts live in `bin/data/scripts/`.
- Lua script path is controlled by the Lua renderer (`script path` parameter / browse action).
- Lua bindings can be regenerated from `src/`:

```bash
cd src
./generate_bindings.sh
```

## Notes

- The app includes project-specific assets and settings under `bin/data/`, behavior depends on those files.
- Generated SWIG wrapper files are included in `src/*Bindings_wrap.cxx`.
