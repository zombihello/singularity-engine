@ECHO OFF
python premake5/premake5.py vs2022 --workspace-on-root %*
python generate_engine_cpp_classes.py
PAUSE