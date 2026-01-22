::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: The batch file to compile shader cache for stdshaders
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@ECHO OFF
SETLOCAL EnableDelayedExpansion

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SET ROOT_DIR=%~dp0/../../../
SET GAME_DIR=%ROOT_DIR%/game/
SET SHADERCOMPILER_EXE=%ROOT_DIR%/src/devtools/shadercompiler/bin/win64_release/shadercompiler.exe
SET SHADERLIST_MAKEFILE=%ROOT_DIR%/src/materialsystem/stdshaders/shaderlist.makefile
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Compile shader cache
ECHO ==============================================================================
ECHO.
ECHO Compiler shader cache for 'stdshaders'
ECHO.
ECHO ==============================================================================

IF NOT EXIST %SHADERCOMPILER_EXE% (
	ECHO ERROR: Shader compiler not found
	GOTO :EXIT
)

"%SHADERCOMPILER_EXE%" -mode compile -file "%SHADERLIST_MAKEFILE%" -backend shadercompiler_vk -output "%GAME_DIR%/core/shaders/stdshaders" %*

ECHO ==============================================================================