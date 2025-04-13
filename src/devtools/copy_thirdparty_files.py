import argparse
from builder import compiler
import thirdparty

# Execute the script
if __name__ == "__main__":
    # Initialize and parse arguments
    argsParser              = argparse.ArgumentParser(  description="Deploy Build (sandbox)" )
    argsParser.add_argument( "build_platform",          help="Build platform" )
    argsParser.add_argument( "build_configuration",     help="Build configuration" )
    argsParser.add_argument( "--build_path", "-o",      help="Path to build directory" )
    argsParser.add_argument( "--game-dir", "-g",        help="Game directory. If isn't set all third parties for a game won't be copied" )
    argsParser.add_argument( "--copy-tools", "-t",      help="Is need copy third party tools", action="store_true" )
    args                    = argsParser.parse_args()

    buildConfiguration      = None
    buildPlatform           = None
    try:
        buildConfiguration  = compiler.Configuration( args.build_configuration.capitalize() )
        buildPlatform       = compiler.Platform( args.build_platform.capitalize() )
    except ValueError as exception:
        raise RuntimeError( f"Invalid build configuration or platform: {exception}" )
    
    # Copy third party files
    _thirdParty = thirdparty.ThirdParty( repoRoot="../../", buildPlatform=buildPlatform, buildConfiguration=buildConfiguration )
    _thirdParty.CopyFiles( buildDir=args.build_path, isCopyTools=args.copy_tools, gameDir=args.game_dir )
