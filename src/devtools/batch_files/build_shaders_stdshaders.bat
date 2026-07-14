::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: The batch file to compile shader cache for stdshaders
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@ECHO OFF
SETLOCAL EnableDelayedExpansion

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SET ROOT_DIR=%~dp0/../../../
SET GAME_DIR=%~1
SET SHADERCOMPILER_EXE=%ROOT_DIR%/src/devtools/shadercompiler/bin/win64_release/shadercompiler.exe
SET SHADERLIST_MAKEFILE=%ROOT_DIR%/src/materialsystem/stdshaders/shaderlist.makefile
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Compile shader cache
ECHO ==============================================================================
ECHO.
ECHO Compiler shader cache for 'stdshaders'
ECHO.
ECHO ==============================================================================

:: Set default game directory if it isn't set
IF "%GAME_DIR%"=="" (
	SET GAME_DIR=%ROOT_DIR%
)
SET GAME_DIR=%GAME_DIR%/game/

:: Make sure that the shader compiler is exist
IF NOT EXIST %SHADERCOMPILER_EXE% (
	ECHO ERROR: Shader compiler not found
	GOTO :EXIT
)

:: Compile shaders
"%SHADERCOMPILER_EXE%" -mode compile -file "%SHADERLIST_MAKEFILE%" -backend shadercompiler_vk -output "%GAME_DIR%/core/shaders"

ECHO ==============================================================================