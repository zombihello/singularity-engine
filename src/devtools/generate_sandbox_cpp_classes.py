from builder import builder
from builder import compiler

# Execute the script
if __name__ == "__main__":
    _builder = builder.Builder( repoRoot="../../" )
    _builder.AddStep_CompileEcs( "Compile ECS (sandbox)", [("src/games/sandbox/", "sandbox/")], useDeployIntermediantDir=False )
    _builder.Execute()
