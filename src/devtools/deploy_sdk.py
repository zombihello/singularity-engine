import argparse
from automation_tool import automation_tool
from automation_tool import compiler
from automation_tool import step
from automation_tool import utils

# Deploy Singularity SDK
def _DeployBuildStep( context ):
    automationTool = context.get("automation_tool")
    utils.DeleteFiles( automationTool.buildDir, utils.GetGarbageFileExtensions() )

# Execute the script
if __name__ == "__main__":
    # Initialize and parse arguments
    argsParser              = argparse.ArgumentParser(  description="Deploy Singularity SDK" )
    argsParser.add_argument( "--output_path", "-o",     help="Output path where will be a build" )
    argsParser.add_argument( "--rebuild", "-r",         help="Do need to build a build from scratch", action="store_true" )
    args                    = argsParser.parse_args()

    # Initialize and start builder
    automationTool = automation_tool.AutomationTool(  repoRoot="../../", 
                                                      buildConfiguration=utils.Configuration.RELEASE, 
                                                      isRebuild=args.rebuild, 
                                                      buildDir=args.output_path )
    automationTool.AddStep_DeleteBuildDir()
    automationTool.AddStep_GenerateProjectFiles()
    automationTool.AddStep_GenerateShaderCppClasses( "Generate Shader C++ Classes (Engine)", automation_tool.ENGINE_CPP_SHADERLISTS )
    automationTool.AddStep_CompileProjects( "Build Tools", automation_tool.TOOL_PROJECTS )
    automationTool.AddStep_CompileProjects( "Build Engine", automation_tool.ENGINE_PROJECTS )
    automationTool.AddStep_CopyThirdPartyFiles( "Copy Third Party Files", isCopyTools=True )
    automationTool.AddStep_CompileShaders( "Compile Engine Shaders", automation_tool.ENGINE_SHADERLISTS, "shadercompiler_vk" )
    automationTool.AddStep_CompileContent( "Compile Engine Content", automation_tool.ENGINE_RESOURCELISTS )
    automationTool.AddStep_MakeLegalNoticesFile( "Make Legal Notices File", isWithTools=True )
    automationTool.AddStep_Custom( step.Step( "Deploy Singularity SDK", _DeployBuildStep, automation_tool=automationTool ) )
    automationTool.Execute()
