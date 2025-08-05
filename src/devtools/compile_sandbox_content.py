from automation_tool import automation_tool
from automation_tool import compiler

# Execute the script
if __name__ == "__main__":
    automationTool = automation_tool.AutomationTool( repoRoot="../../" )
    automationTool.AddStep_CompileContent( "Compile Game (sandbox) Content", ["content/sandbox/resourcelist.txt"] )
    automationTool.Execute()
