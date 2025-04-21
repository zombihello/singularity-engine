from builder import builder
from builder import compiler

# Execute the script
if __name__ == "__main__":
    _builder = builder.Builder( repoRoot="../../" )
    _builder.AddStep_GenerateShaderCppClasses( "Generate Shader C++ Classes (Engine)", builder.ENGINE_CPP_SHADERLISTS, useDeployIntermediantDir=False )
    _builder.AddStep_CompileEcs( "Compile ECS (Engine)", builder.ENGINE_ECS_DIRS, useDeployIntermediantDir=False )
    _builder.Execute()
