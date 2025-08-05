import argparse
from automation_tool import utils
from automation_tool import thirdparty

# Execute the script
if __name__ == "__main__":
    # Initialize and parse arguments
    argsParser              = argparse.ArgumentParser(  description="Copy Third Party Files" )
    argsParser.add_argument( "build_platform",          help="Build platform" )
    argsParser.add_argument( "build_configuration",     help="Build configuration" )
    argsParser.add_argument( "--build_path", "-o",      help="Path to build directory" )
    argsParser.add_argument( "--game-dir", "-g",        help="Game directory. If isn't set all third parties for a game won't be copied" )
    argsParser.add_argument( "--copy-tools", "-t",      help="Is need copy third party tools", action="store_true" )
    args                    = argsParser.parse_args()

    buildConfiguration      = None
    buildPlatform           = None
    try:
        buildConfiguration  = utils.Configuration( args.build_configuration.capitalize() )
        buildPlatform       = utils.Platform( args.build_platform.capitalize() )
    except ValueError as exception:
        raise RuntimeError( f"Invalid build configuration or platform: {exception}" )
    
    # Copy third party files
    thirdParty = thirdparty.ThirdParty( repoRoot="../../", buildPlatform=buildPlatform, buildConfiguration=buildConfiguration )
    thirdParty.CopyFiles( buildDir=args.build_path, isCopyTools=args.copy_tools, gameDir=args.game_dir )
