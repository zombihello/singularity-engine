::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: The batch file to compile all dev tools
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@ECHO OFF
SETLOCAL EnableDelayedExpansion

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SET ROOT_DIR="%~dp0/../../.."
SET BUILD_DIR="%~dp0/../../build/__temp__"
SET BUILD_CONFIGURATION=%~1
SET BUILD_TARGETS=shadercompiler ecscompiler
SET INSTALL_DIR=%~2
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

ECHO ==============================================================================
ECHO.
ECHO Compile dev tools
ECHO.
ECHO ==============================================================================

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
	SET ERRORLEVEL=1
	GOTO :CLEANUP
)

:: Make directory for build
IF NOT EXIST %BUILD_DIR% (
	MKDIR %BUILD_DIR%
)

:: Change current directory
PUSHD %BUILD_DIR% || GOTO :CLEANUP

:: Configure cmake
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" -DCMAKE_BUILD_TYPE=%BUILD_CONFIGURATION% -DINSTALL_THIRDPARTY_TOOLS=OFF -DINSTALL_SDK_TOOLS=OFF -DCMAKE_TOOLCHAIN_FILE="%ROOT_DIR%/src/cmake/toolchains/windows/msvc_toolchain.cmake" "%ROOT_DIR%/src"
IF ERRORLEVEL 1 GOTO :CLEANUP

:: Build and install tools
for %%T in (%BUILD_TARGETS%) do (
    cmake --build . --target %%T || GOTO :CLEANUP
    cmake --install . --component %%T || GOTO :CLEANUP
)

:CLEANUP
POPD
RMDIR /s /q %BUILD_DIR%
ECHO ==============================================================================
IF %ERRORLEVEL% NEQ 0 PAUSE