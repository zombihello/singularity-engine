::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: The batch file to compile resources for citadel
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@ECHO OFF
SETLOCAL EnableDelayedExpansion

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SET ROOT_DIR=%~dp0/../../
SET GAME_DIR=%ROOT_DIR%/game/
SET RESOURCECOMPILER_EXE=%~dp0/../../game/bin/win64_release/resourcecompiler.exe
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Compile resources
ECHO ==============================================================================
ECHO.
ECHO Citadel: compile resources
ECHO.
ECHO ==============================================================================

"%RESOURCECOMPILER_EXE%" -filelist %~dp0/resourcelist.txt -output "%GAME_DIR%/citadel/

ECHO ==============================================================================