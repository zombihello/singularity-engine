from builder import builder
from builder import compiler

# Execute the script
if __name__ == "__main__":
    _builder = builder.Builder( repoRoot="../../" )
    _builder.AddStep_CompileContent( "Compile Engine Content", builder.ENGINE_RESOURCELISTS )
    _builder.Execute()
