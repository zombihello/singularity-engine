::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: The batch file to compile all dev tools
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@ECHO OFF
SETLOCAL EnableDelayedExpansion

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SET ROOT_DIR="%~dp0/../../.."
SET BUILD_DIR="%~dp0/../../build/__temp__"
SET BUILD_DEVTOOLS=shadercompiler ecscompiler
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

ECHO ==============================================================================
ECHO.
ECHO Compile dev tools
ECHO.
ECHO ==============================================================================

:: Make directory for build
IF NOT EXIST %BUILD_DIR% (
	MKDIR %BUILD_DIR%
)

:: Change current directory
PUSHD %BUILD_DIR% || GOTO :CLEANUP

:: Configure cmake
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX="%ROOT_DIR%" -DCMAKE_BUILD_TYPE=Release -DINSTALL_THIRDPARTY_TOOLS=OFF -DCMAKE_TOOLCHAIN_FILE="%ROOT_DIR%/src/cmake/toolchains/windows/msvc_toolchain.cmake" "%ROOT_DIR%/src"
IF ERRORLEVEL 1 GOTO :CLEANUP

:: Build and install tools
for %%T in (%BUILD_DEVTOOLS%) do (
    cmake --build . --target %%T || GOTO :CLEANUP
    cmake --install . --component %%T || GOTO :CLEANUP
)

:CLEANUP
POPD
RMDIR /s /q %BUILD_DIR%
ECHO ==============================================================================