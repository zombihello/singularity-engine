import argparse
import subprocess
import platform
import os

# Execute premake5
def ExecPremake5( action, args=None ):
    subprocess.run( [f"premake5/premake5", action, "--file=../premake5.lua"] + ( args if args else [ "" ] ), check=True )

# Get premake5 action to generate project files depending on current platform
def GetActionToGenerateProjectFiles():
    # Windows (Visual Studio)
    if platform.system() == "Windows":
        return "vs2022"

    # Otherwise it is unknown platform
    else:
        raise RuntimeError( "Unknown platform ", platform.system() )

# Execute the script
if __name__ == "__main__":
    # Initialize and parse arguments
    argsParser  = argparse.ArgumentParser( description="Execute premake5" )
    argsParser.add_argument( "action", help="Premake5 action" )
    argsParser.add_argument( "remainder", nargs=argparse.REMAINDER, help="Extra premake5 arguments" )
    args        = argsParser.parse_args()

    # Change directory to src/devtools
    os.chdir( os.path.join( os.path.dirname( __file__ ), "../" ) )

    # Execute premake5
    try:
        ExecPremake5( args.action, args.remainder )
    except Exception as exception:
        print( f"Exception: {exception}" )