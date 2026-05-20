::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: The batch file to compile and deploy citadel
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@ECHO OFF
SETLOCAL EnableDelayedExpansion

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SET ROOT_DIR=%~dp0/../../..
SET GAME_DIR=%ROOT_DIR%/game/
SET BUILD_CONFIGURATION=%~1
SET INSTALL_DIR=%~2
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Set default build configuration if it isn't set
IF "%BUILD_CONFIGURATION%"=="" (
	SET BUILD_CONFIGURATION=Release
)

:: Set default install directory if it isn't set
IF "%INSTALL_DIR%"=="" (
	SET INSTALL_DIR=%ROOT_DIR%
)

:: Make sure that the build configuration is valid 
IF /i NOT "%BUILD_CONFIGURATION%"=="Debug" ^
IF /i NOT "%BUILD_CONFIGURATION%"=="Release" ^
IF /i NOT "%BUILD_CONFIGURATION%"=="Retail" (
	ECHO ERROR: Invalid build configuration: '%BUILD_CONFIGURATION%'
	ECHO ERROR: Valid configurations: Debug, Release, Retail
	GOTO :CLEANUP
)

:: Build citadel
CALL build_citadel.bat "%BUILD_CONFIGURATION%" "%INSTALL_DIR%"

:: Compile shaders
CALL build_shaders_default.bat "%INSTALL_DIR%"
CALL build_shaders_stdshaders.bat "%INSTALL_DIR%"

:: Compile resources
CALL %ROOT_DIR%/content/core/compile_content.bat "%INSTALL_DIR%"
CALL %ROOT_DIR%/content/citadel/compile_content.bat "%INSTALL_DIR%"

ECHO ==============================================================================
ECHO.
ECHO Deploy citadel
ECHO.
ECHO ==============================================================================

ROBOCOPY "%GAME_DIR%/citadel/" "%INSTALL_DIR%/game/citadel/" "gameinfo.txt"
ROBOCOPY "%GAME_DIR%/citadel/cfg/" "%INSTALL_DIR%/game/citadel/cfg/"

ECHO ==============================================================================