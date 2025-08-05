from automation_tool import automation_tool
from automation_tool import compiler

# Execute the script
if __name__ == "__main__":
    automationTool = automation_tool.AutomationTool( repoRoot="../../" )
    automationTool.AddStep_CompileShaders( "Compile Engine Shaders", automation_tool.ENGINE_SHADERLISTS, "shadercompiler_vk" )
    automationTool.Execute()
