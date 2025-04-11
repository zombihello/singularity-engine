from builder import builder
from builder import compiler

# Execute the script
if __name__ == "__main__":
    _builder = builder.Builder( repoRoot="../../" )
    _builder.AddStep_CompileShaders( "Compile Engine Shaders", builder.ENGINE_SHADERLISTS, "shadercompiler_vk" )
    _builder.Execute()
