import os
import platform
from enum import Enum

# Build configurations
class Configuration( Enum ):
    DEBUG     = "Debug"
    RELEASE   = "Release"
    RETAIL    = "Retail"

# Build platform
class Platform( Enum ):
    WIN64     = "Win64"


# Delete files with specific extensions 
def DeleteFiles(rootDir, extensions, ignoreDirs=None):
    if ignoreDirs is None:
        ignoreDirs = []
    ignoreDirs = {os.path.normpath( os.path.join( rootDir, dir ) ) for dir in ignoreDirs}
    
    for root, dirs, files in os.walk( rootDir ):
        if any( os.path.commonpath ( [root, ignoreDir] ) == ignoreDir for ignoreDir in ignoreDirs ):
            continue

        for file in files:
            if file.endswith( tuple( extensions ) ):
                os.remove( os.path.join( root, file ) )

# Get executable extension
def GetExecutableExtension():
    match platform.system():
        case "Windows":             return ".exe"
        case _:                     raise RuntimeError( "Unknown platform ", platform.system() )

# Get dynamic library extension
def GetDynamicLibraryExtension():
    match platform.system():
        case "Windows":             return ".dll"
        case _:                     raise RuntimeError( "Unknown platform ", platform.system() )

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