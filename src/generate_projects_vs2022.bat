@ECHO OFF
PUSHD %~dp0

CD devtools
"premake5/premake5.exe" vs2022 --file=../premake5.lua --workspace-on-root %*
python generate_engine_cpp_classes.py
python generate_sandbox_cpp_classes.py

POPD
PAUSE