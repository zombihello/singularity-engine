import argparse
import subprocess
import platform
import os

# Execute premake5
def ExecPremake5( action, args=None ):
    originalCwd     = os.getcwd()
    rootDir         = os.path.dirname( os.path.abspath( __file__ ) )
    srcDir  		= os.path.abspath( os.path.join( rootDir, '..', '..' ) )

    try:
        os.chdir( srcDir )
        subprocess.run( [f"devtools/premake5/premake5", action, "--file=premake5.lua"] + ( args if args else [ "" ] ), check=True )
    finally:
        os.chdir( originalCwd )

# Get premake5 action to generate project files depending on current platform
def GetActionToGenerateProjectFiles():
    # Windows (Visual Studio)
    if platform.system() == "Windows":
        return "vs2022"

    # Otherwise it is unknown platform
    else:
        raise RuntimeError( "Unknown platform ", platform.system() )