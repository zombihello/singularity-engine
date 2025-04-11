from builder import builder
from builder import compiler

# Execute the script
if __name__ == "__main__":
    _builder = builder.Builder( repoRoot="../../" )
    _builder.AddStep_CompileContent( "Compile Game (sandbox) Content", ["content/sandbox/resourcelist.txt"] )
    _builder.Execute()
