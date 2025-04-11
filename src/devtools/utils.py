import os
import platform

# Delete files with specific extensions 
def DeleteFiles( rootDir, extensions ):
    for root, dirs, files in os.walk( rootDir ):
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