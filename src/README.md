# src/

| Directory | Builds | Script |
|---|---|---|
| `engine/lithtech/` (+ `engine/server/`) | `lithtech.exe`, the engine | `build/engine.sh lithtech`, `lithtech-link` |
| `engine/server/` | `server.dll`, the game server | `build/engine.sh server`, `server-link` |
| `engine/d3d/` | `d3d.ren`, the Direct3D renderer (Intel C/C++ 4.0) | `build/engine.sh d3d`, `d3d-link` |
| `engine/launcher/` | legacy: `NOLF.exe`, the launcher; modern: `NOLFLauncher.exe` and direct-start `NOLF.exe` | `build/engine.sh launcher link` |
| `engine/ltmsg/` | `ltmsg.dll`, the engine's message strings | `build/engine.sh ltmsg` |
| `engine/sres/` | `sres.dll`, the server resource DLL | `build/engine.sh sres` |
| `lithshared/` | the static libraries the above link | `build/libs.sh all` |
| `game/` | `CRes.dll`, `CShell.dll`, `Object.lto` (Monolith's NOLF v1.003 source) | `build/build.sh all final` |
| `LT2/` | headers only: the LithTech SDK (`sdk/inc`) and shared-library headers (`lithshared/`) | |
| `nolfsrc/` | headers only, ours: the debug build's logging (`nolfsrc_debug.h`) and crash handler (`nolfsrc_crash.h`); on every compiler's include path, and empty in a release build | `make debug` |

`modern/` holds the modern build's own new code, strings and game-data
recipes (`modern/README.md`). Code that departs from the retail game -- bug
fixes, new features -- goes behind `#ifdef NOLFSRC_MODERN`, with the retail code in the `#else`; debug-only
code behind `NOLFSRC_DEBUG`. Neither is ever defined in the release build
(`make modern`, `make debug`; `BUILDING.md`, "Variant builds").

`engine/*/link/` holds each image's committed link inputs (see
`build/engine.sh`). The four source trees sit side by side because the
sources include each other by relative path (`../../LT2/...`), as in
Monolith's release.
