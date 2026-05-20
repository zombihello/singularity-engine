::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: The batch file to compile resources for citadel
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@ECHO OFF
SETLOCAL EnableDelayedExpansion

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SET ROOT_DIR=%~dp0/../../
SET GAME_DIR=%ROOT_DIR%/game/
SET OUTPUT_DIR=%~1
SET RESOURCECOMPILER_EXE=%~dp0/../../game/bin/win64_release/resourcecompiler.exe
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Compile resources
ECHO ==============================================================================
ECHO.
ECHO Citadel: compile resources
ECHO.
ECHO ==============================================================================

:: Set default output directory if it isn't set
IF "%OUTPUT_DIR%"=="" (
	SET OUTPUT_DIR=%ROOT_DIR%
)
SET OUTPUT_DIR=%OUTPUT_DIR%/game/citadel/

:: Make sure that the resource compiler is exist
IF NOT EXIST %RESOURCECOMPILER_EXE% (
	ECHO ERROR: Resource compiler not found
	GOTO :EXIT
)

:: Compile resources
"%RESOURCECOMPILER_EXE%" -game "%GAME_DIR%/citadel/" -filelist "%~dp0/resourcelist.txt" -output "%OUTPUT_DIR%"

ECHO ==============================================================================