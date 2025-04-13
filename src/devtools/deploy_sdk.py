import argparse
from builder import builder
from builder import compiler
from builder import step
import utils

# Deploy Singularity SDK
def _DeployBuildStep( context ):
    _builder = context.get( "builder" )
    utils.DeleteFiles( _builder.buildDir, compiler.GetGarbageFileExtensions() )

# Execute the script
if __name__ == "__main__":
    # Initialize and parse arguments
    argsParser              = argparse.ArgumentParser(  description="Deploy Singularity SDK" )
    argsParser.add_argument( "--output_path", "-o",     help="Output path where will be a build" )
    argsParser.add_argument( "--rebuild", "-r",         help="Do need to build a build from scratch", action="store_true" )
    args                    = argsParser.parse_args()

    # Initialize and start builder
    _builder                = builder.Builder(  repoRoot="../../", 
                                                buildConfiguration=compiler.Configuration.RELEASE, 
                                                isRebuild=args.rebuild, 
                                                buildDir=args.output_path )
    _builder.AddStep_DeleteBuildDir()
    _builder.AddStep_GenerateBuildProjectFiles()
    _builder.AddStep_GenerateShaderCppClasses( "Generate Shader C++ Classes (Engine)", builder.ENGINE_CPP_SHADERLISTS )
    _builder.AddStep_CompileProjects( "Build Tools", builder.TOOL_PROJECTS )
    _builder.AddStep_CompileProjects( "Build Engine", builder.ENGINE_PROJECTS )
    _builder.AddStep_CompileShaders( "Compile Engine Shaders", builder.ENGINE_SHADERLISTS, "shadercompiler_vk" )
    _builder.AddStep_CompileContent( "Compile Engine Content", builder.ENGINE_RESOURCELISTS )
    _builder.AddStep_CopyThirdPartyFiles( "Copy Third Party Files", isCopyTools=True )
    _builder.AddStep_MakeLegalNoticesFile( "Make Legal Notices File", isWithTools=True )
    _builder.AddStep_Custom( step.Step( "Deploy Singularity SDK", _DeployBuildStep, builder=_builder ) )
    _builder.Execute()
