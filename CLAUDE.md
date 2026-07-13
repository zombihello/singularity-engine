# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

Citadel is "Singularity Engine" — a from-scratch, Source-Engine-style modular C++17 game engine
(Windows/MSVC/Vulkan only). Engine subsystems are separate DLLs that discover each other at
runtime through a `CreateInterface` factory pattern (no cross-DLL linking). Gameplay is built on
an ECS (`flecs`) driven by a custom `.ecs` DSL that gets compiled to C++ by an in-tree tool
(`ecscompiler`). Content (materials/textures/models/maps/entities) is authored as human-readable
KeyValues-style source files and compiled to binary runtime formats by another in-tree tool
(`resourcecompiler`).

## Build system

Windows-only, MSVC + Ninja, no other generator/compiler is supported (`src/CMakeLists.txt` emits
`SEND_ERROR` for anything else). Three build configurations exist: `Debug`, `Release`, `Retail`
(there is no plain "RelWithDebInfo" — `Retail` is the shipping-optimized config, `/Ox`, no `/Zi`
line implied by the Release config's debug info being stripped).

CMake source root is `src/` (not the repo root). Toolchain file is
`src/cmake/toolchains/windows/msvc_toolchain.cmake`.

### Manual configure/build (from `src/devtools/batch_files/`, the pattern every `.bat` uses)

```bat
cmake -G "Ninja" ^
  -DCMAKE_INSTALL_PREFIX="<install_dir>" ^
  -DCMAKE_BUILD_TYPE=<Debug|Release|Retail> ^
  -DCMAKE_TOOLCHAIN_FILE="src/cmake/toolchains/windows/msvc_toolchain.cmake" ^
  "src"

cmake --build . --target <target_name>
cmake --install . --component <target_name>
```

Main targets: `citadel` (the game DLL, output name `game.dll`), `citadel_launcher` (the exe),
`shadercompiler`, `ecscompiler`, `resourcecompiler` (dev/SDK tools), plus one DLL per engine
subsystem (`filesystem`, `cvar`, `inputsystem`, `studioapi_vk`, `studiorender`, `materialsystem`,
`modelsystem`, `resourcesystem`, `launcher`, ...) — see `src/CMakeLists.txt` for the full
`add_subdirectory` list.

### Preferred entry points — `src/devtools/batch_files/*.bat`

- `build_citadel.bat [Debug|Release|Retail] [install_dir]` — configures + builds + installs
  `citadel` and `citadel_launcher`. Defaults: config `Release`, install dir = repo root.
- `build_dev_tools.bat [config] [install_dir]` — builds `shadercompiler` + `ecscompiler`.
- `build_sdk_tools.bat [config] [install_dir]` — builds `resourcecompiler`.
- `build_shaders_default.bat [install_dir]` / `build_shaders_stdshaders.bat [install_dir]` — run
  the **already-built** `shadercompiler.exe` against `src/materialsystem/shaderlist.makefile` /
  `src/materialsystem/stdshaders/shaderlist.makefile`, writing `.ssc` shader caches into
  `<install_dir>/game/core/shaders/{default,stdshaders}`.
- `deploy_citadel.bat [config] [install_dir]` — full pipeline: builds citadel, compiles shaders
  (both makefiles), compiles content for `core` and `citadel`
  (`content/{core,citadel}/compile_content.bat`), then robocopies `gameinfo.txt`/`cfg/` into the
  install dir's `game/citadel/`. This is the closest thing to a one-shot "build everything and
  produce a runnable game" command.
- `install_githooks.bat` — copies `src/devtools/githooks/commit-msg` into `.git/hooks/`.

Each batch file uses a scratch build directory (`<repo>/build/__temp__`) and deletes it when done
— builds are not incremental across separate `.bat` invocations. For iterative development,
configure once manually into a persistent build dir and just re-run `cmake --build .`.

### Compiling content directly (after the tools are built)

```bat
resourcecompiler.exe -game "<game_dir>/citadel/" -filelist "content/citadel/resourcelist.txt" -output "<game_dir>/citadel/"
```
Content compilers dispatch by source file extension (`.smat`/`.stex`/`.smdl`/`.sent`/`.smap` →
`.smat_c`/`.stex_c`/`.smdl_c`/`.sent_c`/`.smap_c`); the set of installed compiler plugins is
declared per-game in `game/<name>/toolsinfo.txt`. Runtime never reads `content/` directly, only
the compiled `*_c` files under `game/`.

```bat
shadercompiler.exe -mode compile -file <shaderlist.makefile> -backend shadercompiler_vk -output <dir>
shadercompiler.exe -mode gencpp  -file <shaderlist.makefile> -output <dir>
```

```bat
ecscompiler.exe -file <path.ecs> -output <dir>      REM or -dir <path> for a whole directory
```
`.ecs` → generated `.gen.h`/`.gen.cpp` is normally invoked automatically by CMake
(`add_ecscompiler_commands` in `src/cmake/utils.cmake`) for `game_shared`/`citadel` — you rarely
need to run it by hand.

## Tests

There is no test suite in this repo's own code (only vendored tests inside `src/thirdparty/*`,
which are not part of the build). Verification is currently manual: build, run
`citadel_launcher.exe`, check the loaded test scene.

## Repo conventions

- **Branch → commit message linkage.** The installed `commit-msg` hook
  (`src/devtools/githooks/commit-msg`) extracts a `CIT-<number>` task ID from the current branch
  name and auto-prepends it to the commit message if not already present. Branches follow
  `task/<description>_CIT-<n>` or `bugfix/<description>` naming (see recent git log for examples).
- **Public vs. implementation split.** Every module's public API lives under `src/public/<module>/`
  (included by other modules); the implementation lives in `src/<module>/` and is never included
  cross-module. When adding a new cross-module-visible type or interface, it goes in `public/`.
- **`content/` vs `game/`.** `content/<name>/` is hand-authored, version-controlled source
  (KeyValues text, `.fbx`, images). `game/<name>/` is the compiled runtime tree the engine
  actually loads — treat it as a build artifact, not something to hand-edit (compiled resource
  files, textures, DLLs, etc. are tracked via Git LFS per `.gitattributes`, but are still
  generated output).
- Third-party code lives untouched under `src/thirdparty/`; don't attribute engine bugs to it
  without first checking the integration point (usually a `src/<module>/` or `studioapi_vk` file
  that calls into it).
