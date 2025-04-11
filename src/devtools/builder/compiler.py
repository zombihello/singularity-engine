import os
import subprocess
import platform
from enum import Enum

# Build configurations
class Configuration( Enum ):
    DEBUG   = "Debug"
    RELEASE = "Release"
    RETAIL  = "Retail"

# Build platform
class Platform( Enum ):
    WIN64   = "Win64"

# Get compiler path
def GetCompilerPath():
    match platform.system():
        # Windows (Visual Studio)
        case "Windows":
            vswhere_path = os.path.join( os.environ.get( "ProgramFiles(x86)", r"C:\Program Files (x86)" ),
                                        "Microsoft Visual Studio",
                                        "Installer",
                                        "vswhere.exe" )
            find_msbuild_cmd = [vswhere_path,
                                "-latest",
                                "-prerelease",
                                "-products", "*",
                                "-requires", "Microsoft.Component.MSBuild",
                                "-find", "MSBuild\\**\\Bin\\MSBuild.exe"]
            
            # Try to find Visual Studio
            try:
                msbuild_path = subprocess.check_output( find_msbuild_cmd, encoding="utf-8" ).strip()
                print( "MSBuild: ", msbuild_path )
                return msbuild_path
            except subprocess.CalledProcessError as exception:
                raise RuntimeError( "Error executing command: ", exception )
            except FileNotFoundError:
                raise RuntimeError( "vswhere.exe not found. Make sure Visual Studio is installed" )

        # Otherwise it is unknown platform
        case _:
            raise RuntimeError( f"Unknown platform: {platform.system()}" )

# Get workspace file extension
def GetWorkspaceFileExtension():
    match platform.system():
        case "Windows":             return ".sln"
        case _:                     raise RuntimeError( f"Unknown platform: {platform.system()}" )

# Get project file extension
def GetProjectFileExtension():
    match platform.system():
        case "Windows":             return ".vcxproj"
        case _:                     raise RuntimeError( f"Unknown platform: {platform.system()}" )

# Get garbage file extensions
def GetGarbageFileExtensions():
    match platform.system():
        case "Windows":             return [".exp", ".lib", ".pdb"]
        case _:                     raise RuntimeError( f"Unknown platform: {platform.system()}" )

# Get native platform
def GetNativePlatform():
    match platform.system():
        case "Windows":
            match platform.machine().lower():
                case "amd64":       return Platform.WIN64
                case "x86_64":      return Platform.WIN64
                case _:             raise RuntimeError( f"Unknown platform bit: {platform.machine()}" )
        case _:                     raise RuntimeError( f"Unknown platform: {platform.system()}" )

# Translate compile configuration
def _TranslateCompileConfiguration( buildConfiguration, buildPlatform ):
    match platform.system():
        case "Windows":             return f"{buildConfiguration.value} {buildPlatform.value}"   
        case _:                     raise RuntimeError( f"Unknown platform: {platform.system()}" )

# Translate compile platform
def _TranslateCompilePlatform( buildPlatform ): 
    match platform.system():
        # Windows (Visual Studio)
        case "Windows":
            match buildPlatform.value:
                case "Win64":       return "x64"
                case "Win32":       return "Win32"
                case _:             raise RuntimeError( f"Unknown build platform: {buildPlatform.value}" )
        
         # Otherwise it is unknown platform
        case _:
            raise RuntimeError( f"Unknown platform: {platform.system()}" )

# Initialize some constants
COMPILER_PATH               = GetCompilerPath()
WORKSPACE_FILE_EXTENSION    = GetWorkspaceFileExtension()
PROJECT_FILE_EXTENSION      = GetProjectFileExtension()

# Compile project
def CompileProject( projectPath, buildConfiguration, buildPlatform, isRebuild=None ):
    # Make build commmand
    build_cmd = None
    match platform.system():
        # Windows (Visual Studio)
        case "Windows":
            build_cmd = [COMPILER_PATH, projectPath,
                         f"/t:Rebuild" if isRebuild else "/t:Build",
                         f"/p:Configuration={_TranslateCompileConfiguration( buildConfiguration, buildPlatform )}", 
                         f"/p:Platform={_TranslateCompilePlatform( buildPlatform )}"]

        # Otherwise it is unknown platform
        case _:
            raise RuntimeError( f"Unknown platform: {platform.system()}" )
    
    # Compile a project
    subprocess.run( build_cmd, check=True )