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
- Procedural shader renderer with transactional GLSL hot-reload.
- Character set + font based ASCII conversion.
- Overlay/grid/flip effects and theme support.
- Optional shader post-processing before ASCII conversion.
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

## Post-processing

The `post effect` GUI group applies `bin/data/shaders/post/post.vert` and
`post.frag` to the renderer canvas before it is downscaled and converted to
ASCII. Use `intensity` to blend the shader output, `distortion` to control the
wave displacement, and `speed` to control its animation rate.

If the shader cannot be loaded, the app logs an error and converts the original
canvas instead.

## Shader Renderer

Press `/` to add a shader renderer. Its `shader path` parameter and `browse
shader` button select a `.frag` file under `bin/data/`; a same-basename `.vert`
file is used when present, otherwise a built-in pass-through vertex shader is
used. Changes to either file are detected automatically. A shader is activated
only after it compiles and links successfully, so an invalid edit leaves the
last valid shader running.

Fragment shaders can receive `time`, `canvasSize`, `dimensions`, `color`,
`position`, `rotation`, `modulation`, `modulation_count`, `resolution`, and
`particle_count` uniforms. The example shader uses position XY, dimensions XY,
rotation Z, and particle count as 2D procedural controls. See
`bin/data/shaders/render/generative.frag` for an example.

## Notes

- The app includes project-specific assets and settings under `bin/data/`, behavior depends on those files.
- Generated SWIG wrapper files are included in `src/*Bindings_wrap.cxx`.
