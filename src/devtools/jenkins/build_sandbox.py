import sys
import os
import shutil
sys.path.append( os.path.dirname( os.path.dirname( os.path.abspath( __file__ ) ) ) )

import argparse
from builder import builder
from builder import compiler
from builder import step
import utils

# Copy files and cleanup a build directory
def _CopyFilesAndCleanupStep( context ):
    _builder          = context.get( "builder" )
    srcGameDir        = f"{_builder.repoRoot}/game/sandbox"
    dstGameDir        = f"{_builder.buildDir}/sandbox"

    # Copy gameinfo.txt and config.cfg
    print( srcGameDir )
    print( dstGameDir )

    if os.path.isdir( srcGameDir ) and ( not os.path.isdir( dstGameDir ) or not os.path.samefile( srcGameDir, dstGameDir ) ):
        srcGameInfoFile     = f"{srcGameDir}/gameinfo.txt"
        dstGameInfoFile     = f"{dstGameDir}/gameinfo.txt"
        srcCfgFile          = f"{srcGameDir}/cfg/config.cfg"
        dstCfgFile          = f"{dstGameDir}/cfg/config.cfg"
        shutil.copy( srcGameInfoFile, dstGameInfoFile )
        os.makedirs( os.path.dirname( dstCfgFile ), exist_ok=True )
        shutil.copy( srcCfgFile, dstCfgFile )
        os.chmod( dstGameInfoFile, 0o666 )
        os.chmod( dstCfgFile, 0o666 )

    utils.DeleteFiles( _builder.buildDir, compiler.GetGarbageFileExtensions() )


# Execute the script
if __name__ == "__main__":
    # Initialize and parse arguments
    argsParser = argparse.ArgumentParser( description="Build Sandbox" )
    argsParser.add_argument( "build_platform",                      help="Build platform" )
    argsParser.add_argument( "build_configuration",                 help="Build configuration" )
    argsParser.add_argument( "--output_path",                       help="Output path where will be a build" )
    argsParser.add_argument( "--rebuild",                           help="Do need to build a build from scratch", action="store_true" )
    argsParser.add_argument( "--with-sdk",                          help="Build engine with Singularity SDK", action="store_true" )
    argsParser.add_argument( "--generate-project-files",            help="Generate project files", action="store_true" )
    argsParser.add_argument( "--generate-shadercpp-classes",        help="Generate shader C++ classes", action="store_true" )
    argsParser.add_argument( "--compile-ecs",                       help="Compile ECS files", action="store_true" )
    argsParser.add_argument( "--compile-cpp",                       help="Compile C++ code", action="store_true" )
    argsParser.add_argument( "--compile-content",                   help="Compile content", action="store_true" )
    argsParser.add_argument( "--copy-files-and-cleanup",            help="Copy files and cleanup the build directory", action="store_true" )
    args                    = argsParser.parse_args()

    buildConfiguration      = None
    buildPlatform           = None
    try:
        buildConfiguration  = compiler.Configuration( args.build_configuration.capitalize() )
        buildPlatform       = compiler.Platform( args.build_platform.capitalize() )
    except ValueError as exception:
        raise RuntimeError( f"Invalid build configuration or platform: {exception}" )

    # Initialize and start builder
    _builder                = builder.Builder(  repoRoot="../../", 
                                                buildDir=args.output_path,
                                                buildPlatform=buildPlatform, 
                                                buildConfiguration=buildConfiguration,
                                                isRebuild=args.rebuild )
    
    # Generate project files
    if args.generate_project_files:
        _builder.AddStep_GenerateBuildProjectFiles( gameName="sandbox" )

    # Generate shader C++ classes
    if args.generate_shadercpp_classes:
        _builder.AddStep_GenerateShaderCppClasses( "Generate Shader C++ Classes (Engine)", builder.ENGINE_CPP_SHADERLISTS )

    # Compile ECS files
    if args.compile_ecs:
        _builder.AddStep_CompileEcs( "Compile ECS (Engine)", builder.ENGINE_ECS_DIRS )
        _builder.AddStep_CompileEcs( "Compile ECS (sandbox)", [("src/games/sandbox/", "games/sandbox/")] )

    # Compile C++ code
    if args.compile_cpp:
        if args.with_sdk:
            _builder.AddStep_CompileProjects( "Compile Singularity SDK", builder.TOOL_PROJECTS )
        _builder.AddStep_CompileProjects( "Compile Engine", builder.ENGINE_PROJECTS )
        _builder.AddStep_CompileProjects( "Compile Game (sandbox)", ["sandbox", "sandbox_launcher"] )

    # Compile content
    if args.compile_content:
        _builder.AddStep_CompileShaders( "Compile Engine Shaders", builder.ENGINE_SHADERLISTS, "shadercompiler_vk" )
        _builder.AddStep_CompileContent( "Compile Engine Content", builder.ENGINE_RESOURCELISTS )
        _builder.AddStep_CompileContent( "Compile Game (sandbox) Content", ["content/sandbox/resourcelist.txt"] )

    # Copy files and cleanup the build directory
    if args.copy_files_and_cleanup:
        _builder.AddStep_CopyThirdPartyFiles( "Copy Third Party Files", isCopyTools=args.with_sdk, gameDir="sandbox" )
        _builder.AddStep_MakeLegalNoticesFile( "Make Legal Notices File", isWithTools=args.with_sdk )
        _builder.AddStep_Custom( step.Step( "Copy Files And Cleanup The Build Directory", _CopyFilesAndCleanupStep, builder=_builder ) )

    # Executre the builder
    _builder.Execute()