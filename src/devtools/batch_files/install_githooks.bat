::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: The batch file to install Git hooks
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@ECHO OFF
SETLOCAL EnableDelayedExpansion
PUSHD "%~dp0"

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SET ROOT_DIR="%~dp0/../../.."
SET GIT_HOOKS_SRC_DIR=%ROOT_DIR%\src\devtools\githooks
SET GIT_HOOKS_DEST_DIR=%ROOT_DIR%\.git\hooks
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Make sure that we have git hooks directory
IF NOT EXIST %GIT_HOOKS_DEST_DIR% GOTO NO_GIT_HOOKS_DIRECTORY

:: Install git hooks
ECHO Install git hooks
COPY %GIT_HOOKS_SRC_DIR%\commit-msg %GIT_HOOKS_DEST_DIR%\commit-msg
GOTO END

:: Message about no git hooks directory
:NO_GIT_HOOKS_DIRECTORY
ECHO WARNING: Couldn't install git hooks, directory '%GIT_HOOKS_DEST_DIR%' isn't found
GOTO END

:: Error happened. Wait for a keypress before quitting.
:ERROR
PAUSE

:: End of the bat script
:END
POPD