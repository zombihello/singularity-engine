::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: The batch file to compile shader cache for default
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@ECHO OFF
SETLOCAL EnableDelayedExpansion

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SET GAME_DIR=%~dp0/../../game/
SET SHADERCOMPILER_EXE=%~dp0/../devtools/shadercompiler/bin/win64_release/shadercompiler.exe
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Compile shader cache
ECHO ==============================================================================
ECHO.
ECHO Default: compile shader cache
ECHO.
ECHO ==============================================================================

"%SHADERCOMPILER_EXE%" -mode compile -file %~dp0/shaderlist.makefile -backend shadercompiler_vk -output "%GAME_DIR%/core/shaders/default"

ECHO ==============================================================================