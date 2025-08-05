from automation_tool import automation_tool
from automation_tool import compiler

# Execute the script
if __name__ == "__main__":
    automationTool = automation_tool.AutomationTool( repoRoot="../../" )
    automationTool.AddStep_GenerateShaderCppClasses( "Generate Shader C++ Classes (Engine)", automation_tool.ENGINE_CPP_SHADERLISTS, useDeployIntermediantDir=False )
    automationTool.AddStep_CompileEcs( "Compile ECS (Engine)", automation_tool.ENGINE_ECS_DIRS, useDeployIntermediantDir=False )
    automationTool.Execute()
