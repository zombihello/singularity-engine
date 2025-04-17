import argparse
import os
import shutil
from builder import builder
from builder import compiler
from builder import step
import utils

# Deploy a build
def _DeployBuildStep( context ):
    _builder          = context.get( "builder" )
    srcGameDir        = f"{_builder.repoRoot}/game/sandbox"
    dstGameDir        = f"{_builder.buildDir}/sandbox"

    # Copy gameinfo.txt and config.cfg
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
    argsParser              = argparse.ArgumentParser(  description="Deploy Build (sandbox)" )
    argsParser.add_argument( "build_platform",          help="Build platform" )
    argsParser.add_argument( "build_configuration",     help="Build configuration" )
    argsParser.add_argument( "--output_path", "-o",     help="Output path where will be a build" )
    argsParser.add_argument( "--rebuild", "-r",         help="Do need to build a build from scratch", action="store_true" )
    argsParser.add_argument( "--sdk", "-s",             help="Set custom Singularity SDK to build" )
    argsParser.add_argument( "--with-sdk", "-ws",       help="Build with Singularity SDK", action="store_true" )
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
                                                sdkDir=args.sdk, 
                                                buildDir=args.output_path,
                                                buildPlatform=buildPlatform, 
                                                buildConfiguration=buildConfiguration,
                                                isRebuild=args.rebuild )
    _builder.AddStep_DeleteBuildDir()
    _builder.AddStep_GenerateBuildProjectFiles( gameName="sandbox" )
    _builder.AddStep_GenerateShaderCppClasses( "Generate Shader C++ Classes (Engine)", builder.ENGINE_CPP_SHADERLISTS )
    _builder.AddStep_CompileEcs( "Compile ECS (Engine)", builder.ENGINE_CPP_ECS )
    if args.with_sdk:
        _builder.AddStep_CompileProjects( "Build Singularity SDK", builder.TOOL_PROJECTS )
    _builder.AddStep_CompileProjects( "Build Engine", builder.ENGINE_PROJECTS )
    _builder.AddStep_CompileProjects( "Build Game (sandbox)", ["sandbox", "sandbox_launcher"] )
    _builder.AddStep_CompileShaders( "Compile Engine Shaders", builder.ENGINE_SHADERLISTS, "shadercompiler_vk" )
    _builder.AddStep_CompileContent( "Compile Engine Content", builder.ENGINE_RESOURCELISTS )
    _builder.AddStep_CompileContent( "Compile Game (sandbox) Content", ["content/sandbox/resourcelist.txt"] )
    _builder.AddStep_CopyThirdPartyFiles( "Copy Third Party Files", isCopyTools=args.with_sdk, gameDir="sandbox" )
    _builder.AddStep_MakeLegalNoticesFile( "Make Legal Notices File", isWithTools=args.with_sdk )
    _builder.AddStep_Custom( step.Step( "Deploy Build", _DeployBuildStep, builder=_builder ) )
    _builder.Execute()
