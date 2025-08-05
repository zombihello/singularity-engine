import json
import os
import shutil
from . import premake5
from . import utils

# Third party class
class ThirdParty:
    # Initialize a object
    def __init__( self, repoRoot, buildPlatform, buildConfiguration ):
        self.repoRoot           = repoRoot
        self.buildPlatform      = buildPlatform
        self.buildConfiguration = buildConfiguration

        # Export third party into a JSON file
        premake5.ExecPremake5( "export-thirdparty", ["--automation-tool"] )
        print( "", flush=True )

        # Load JSON file
        with open( f"{self.repoRoot}/src/thirdparty/thirdparty.json", "r", encoding="utf-8" ) as file:
            self.jsonData = json.load( file )

    # Copy files
    def CopyFiles( self, buildDir=None, isCopyTools=False, gameDir=None ):
        buildDir        = buildDir if buildDir else f"{self.repoRoot}/game/"
        engineBinDir    = f"{buildDir}/bin/" + f"{self.buildPlatform.value}_{self.buildConfiguration.value}/".lower()
        if gameDir:
            gameBinDir  = f"{buildDir}/{gameDir}/bin/" + f"{self.buildPlatform.value}_{self.buildConfiguration.value}/".lower()
        else:
            gameBinDir  = None

        for libName, libInfo in self.jsonData.items():
            # Skip the third party if it hasn't been linked
            if not libInfo.get( "IsLinked", False ):
                continue

            # Get path to the third party
            libPath         = libInfo.get( "Path", None )
            if libPath == None:
                raise RuntimeError( f"Third party '{libName}' has invalid 'Path' field" )

            # Skip the third party if we don't copy tools
            libIsTools      = libInfo.get( "IsTools", False )
            if libIsTools and not isCopyTools:
                print( f"Third party '{libName}' skiped, because it marked as tool but we don't copy tools", flush=True )
                continue

            # Is the third party for game only
            libForGameOnly  = libInfo.get( "IsForGameOnly", False )
            if libForGameOnly and gameBinDir == None:
                print( f"Third party '{libName}' skiped, because it marked as game only but a game directory isn't set", flush=True )
                continue

            destBinDir = engineBinDir
            if libForGameOnly:
                destBinDir = gameBinDir

            libPlatformData                = libInfo.get( self.buildPlatform.value, {} )
            if libPlatformData:
                libConfigurationData       = libPlatformData.get( self.buildConfiguration.value, {} )
                if libConfigurationData:
                    # Skip the third party if it isn't used in the configuration
                    if not libConfigurationData.get( "IsUsed", False ):
                        continue

                    # Copy DLLs for the configuration
                    libConfigurationDLLs   = libConfigurationData.get( "DLLs", {} )
                    for dllName in libConfigurationDLLs:
                        print( f"Copy {libPath}/{dllName}", flush=True )
                        os.makedirs( destBinDir, exist_ok=True )
                        shutil.copy( f"{libPath}/{dllName}", f"{destBinDir}/{os.path.basename( dllName )}" )
            
                # Copy general DLLs
                libPlatformDLLs            = libPlatformData.get( "DLLs", {} )
                for dllName in libPlatformDLLs:
                    print( f"Copy {libPath}/{dllName}", flush=True )
                    os.makedirs( destBinDir, exist_ok=True )
                    shutil.copy( f"{libPath}/{dllName}", f"{destBinDir}/{os.path.basename( dllName )}" )

                # Copy tools if it need
                if isCopyTools:
                    libPlatformTools       = libPlatformData.get( "Tools", {} )
                    for toolsName in libPlatformTools:
                        print( f"Copy {libPath}/{toolsName}", flush=True )
                        os.makedirs( destBinDir, exist_ok=True )
                        shutil.copy( f"{libPath}/{toolsName}", f"{destBinDir}/{os.path.basename( toolsName )}" )

    # Make a file with legal notices
    def MakeLegalNoticesFile( self, buildDir=None, isWithTools=False ):
        with open( f"{buildDir}/thirdpartylegalnotices.txt", "w", encoding="utf-8" ) as outputFile:
            outputFile.write( "The Singularity engine and Broken Singularity games use Third Party code for certain functions. The\n" )
            outputFile.write( "required legal notices for using such code are reproduced below in accordance with\n" )
            outputFile.write( "Broken Singularity's obligations to provide such notices:\n" )
            outputFile.write( "\n" )

            for libName, libInfo in self.jsonData.items():
                # Get the third party licenses
                libLicenses                 = libInfo.get( "Licenses", {} )
                if not libLicenses:
                    continue

                # Skip the third party if it hasn't been linked
                if not libInfo.get( "IsLinked", False ):
                    continue

                # Check is using the third party in the configuration
                libIsUsing                  = False
                libPlatformData             = libInfo.get( self.buildPlatform.value, {} )
                if libPlatformData:
                    libConfigurationData    = libPlatformData.get( self.buildConfiguration.value, {} )
                    if libConfigurationData:
                        libIsUsing = libConfigurationData.get( "IsUsed", False )

                # Skip the third party if it isn't used
                if not libIsUsing:
                    print( f"Third party '{libName}' skiped, because it isn't used in configuration or platform: {self.buildConfiguration} {self.buildPlatform}", flush=True )
                    continue

                # Skip the third party if we don't count tools
                libIsTools                  = libInfo.get( "IsTools", False )
                if libIsTools and not isWithTools:
                    print( f"Third party '{libName}' skiped, because it marked as tool but we don't count tools", flush=True )
                    continue

                # Get path to the third party
                libPath                     = libInfo.get( "Path", None )
                if libPath == None:
                    raise RuntimeError( f"Third party '{libName}' has invalid 'Path' field" )

                # Copy a license text into our file
                print( f"Added {libName}", flush=True )
                outputFile.write( "************************************************************************************\n" )
                outputFile.write( f"{libName}\n" )
                outputFile.write( "************************************************************************************\n" )
                for licenseName in libLicenses:
                    if os.path.isdir( f"{libPath}/{licenseName}" ):
                        raise RuntimeError( f"File '{libPath}/{licenseName}' is directory but must be a file" )

                    with open( f"{libPath}/{licenseName}", "r", encoding="utf-8" ) as inputFile:
                        outputFile.write( f">>>>> File: {licenseName} <<<<<\n" )
                        outputFile.write( inputFile.read() )
                        outputFile.write( "\n\n" )