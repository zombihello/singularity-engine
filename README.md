![SingularityEngine](docs/engineLogo.png?raw=true)

## Description
Singularity Engine is a 3D game engine inspired by Source Engine and Quake Engine 

## Building

### Windows
1. Install [Visual Studio 2022 Community](https://visualstudio.microsoft.com/downloads/).
2. Install [Vulkan SDK](https://vulkan.lunarg.com/home/welcome).
3. Install [Python](https://www.python.org/)
4. Run `src/devtools/generate_projects_vs2022.bat` with parameter ```-game <gameName>```. By default this script generate project for <b>'sandbox'</b>.
5. Run `src/devtools/compile_engine_shaders_vk.py`
6. Run `src/devtools/compile_engine_content.py`
7. Run `compile_sandbox_content.py` (optional, it only if you want play in sandbox game)
3. Open solution `src/singularity_engine.sln`.
4. Build project `<gameName>_launcher`.

## Current status
- Not ready for any use
- See [TODO.md](TODO.md)
- See [NOTES.md](NOTES.md)