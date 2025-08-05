import argparse
import os
import shutil
from automation_tool import automation_tool
from automation_tool import compiler
from automation_tool import step
from automation_tool import utils

# Deploy a build
def _DeployBuildStep( context ):
    automationTool      = context.get("automation_tool")
    srcGameDir          = f"{automationTool.repoRoot}/game/sandbox"
    dstGameDir          = f"{automationTool.buildDir}/sandbox"

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

    utils.DeleteFiles( automationTool.buildDir, utils.GetGarbageFileExtensions() )

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
        buildConfiguration  = utils.Configuration( args.build_configuration.capitalize() )
        buildPlatform       = utils.Platform( args.build_platform.capitalize() )
    except ValueError as exception:
        raise RuntimeError( f"Invalid build configuration or platform: {exception}" )
    
    # Initialize and start builder
    automationTool = automation_tool.AutomationTool(  repoRoot="../../", 
                                                      sdkDir=args.sdk, 
                                                      buildDir=args.output_path,
                                                      buildPlatform=buildPlatform, 
                                                      buildConfiguration=buildConfiguration,
                                                      isRebuild=args.rebuild )
    automationTool.AddStep_DeleteBuildDir()
    automationTool.AddStep_GenerateProjectFiles( gameName="sandbox" )
    automationTool.AddStep_GenerateShaderCppClasses( "Generate Shader C++ Classes (Engine)", automation_tool.ENGINE_CPP_SHADERLISTS )
    automationTool.AddStep_CompileEcs( "Compile ECS (Engine)", automation_tool.ENGINE_ECS_DIRS )
    automationTool.AddStep_CompileEcs( "Compile ECS (sandbox)", [("src/games/sandbox/", "games/sandbox/")] )
    if args.with_sdk:
        automationTool.AddStep_CompileProjects( "Build Singularity SDK", automation_tool.TOOL_PROJECTS )
    automationTool.AddStep_CompileProjects( "Build Engine", automation_tool.ENGINE_PROJECTS )
    automationTool.AddStep_CompileProjects( "Build Game (sandbox)", ["sandbox", "sandbox_launcher"] )
    automationTool.AddStep_CompileShaders( "Compile Engine Shaders", automation_tool.ENGINE_SHADERLISTS, "shadercompiler_vk" )
    automationTool.AddStep_CompileContent( "Compile Engine Content", automation_tool.ENGINE_RESOURCELISTS )
    automationTool.AddStep_CompileContent( "Compile Game (sandbox) Content", ["content/sandbox/resourcelist.txt"] )
    automationTool.AddStep_CopyThirdPartyFiles( "Copy Third Party Files", isCopyTools=args.with_sdk, gameDir="sandbox" )
    automationTool.AddStep_MakeLegalNoticesFile( "Make Legal Notices File", isWithTools=args.with_sdk )
    automationTool.AddStep_Custom( step.Step( "Deploy Build", _DeployBuildStep, automation_tool=automationTool ) )
    automationTool.Execute()
