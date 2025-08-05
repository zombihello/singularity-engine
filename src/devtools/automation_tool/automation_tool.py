import sys
import os
import traceback
import shutil
import subprocess
from . import compiler
from . import step
from . import premake5
from . import utils
from . import thirdparty

# Global variables
INTERMEDIATE_ROOT_DIR               = "intermediate/"
INTERMEDIATE_AUTOMATIONTOOL_DIR     = f"{INTERMEDIATE_ROOT_DIR}/automation_tool/"
PREMAKE5_ACTION                     = premake5.GetActionToGenerateProjectFiles()
TOOL_PROJECTS                       = ["core", "cvar", "filesystem", "material_tool", "model_tool", "resourcecompiler", "texture_tool", "shadercompiler", "shadercompiler_vk", "ecscompiler", "entitydesc_tool", "map_tool"]
ENGINE_PROJECTS                     = ["core", "cvar", "filesystem", "inputsystem", "launcher", "materialsystem", "resourcesystem", "studioapi_vk", "studiorender", "stdshaders"]
ENGINE_SHADERLISTS                  = ["src/materialsystem/shaderlist.makefile", "src/materialsystem/stdshaders/shaderlist.makefile"]
ENGINE_CPP_SHADERLISTS              = [("src/materialsystem/shaderlist.makefile", "materialsystem/"), ("src/materialsystem/stdshaders/shaderlist.makefile", "materialsystem/stdshaders/")]
ENGINE_ECS_DIRS                     = [("src/public/libs/gameframework/", "gameframework/")]
ENGINE_RESOURCELISTS                = ["content/core/resourcelist.txt"]
SDK_SHADERCOMPILER_EXE              = f"shadercompiler{utils.GetExecutableExtension()}"
SDK_RESOURCECOMPILER_EXE            = f"resourcecompiler{utils.GetExecutableExtension()}"
SDK_ECSCOMPILER_EXE                 = f"ecscompiler{utils.GetExecutableExtension()}"

# Delete build directory step
def _DeleteBuildDir( context ):
    srcBuildDir     = f"{context.get( "repo_root" )}/game/"
    dstBuildDir     = context.get( "build_dir" )
    if os.path.isdir( srcBuildDir ) and os.path.isdir( dstBuildDir ) and not os.path.samefile( srcBuildDir, dstBuildDir ):
        shutil.rmtree( dstBuildDir )

# Generate project files for build
def _GenerateProjectFiles( context ):
    gameName        = context.get( "game" )
    premake5.ExecPremake5( PREMAKE5_ACTION, ["--automation-tool", f"--intermediate-dir={INTERMEDIATE_AUTOMATIONTOOL_DIR}", f"--build-dir={context.get( "build_dir" )}", f"--game={gameName}" if gameName else ""] )

# Get path to project file
def _GetPathToProjectFile( repoRoot, projectName ):
    return f"{repoRoot}/src/{INTERMEDIATE_AUTOMATIONTOOL_DIR}/{PREMAKE5_ACTION}/{projectName}{utils.GetProjectFileExtension()}"

# Compile projects
def _CompileProjects( context ):
    projects            = context.get( "projects" )
    repoRoot            = context.get( "repo_root" )
    buildConfiguration  = context.get( "build_configuration" )
    buildPlatform       = context.get( "build_platform" )
    isRebuild           = context.get( "is_rebuild" )
    for project in projects:
        print( "" )
        print( f"> Compile: {project}" )
        print( "", flush=True )
        compiler.CompileProject( _GetPathToProjectFile( repoRoot, project ), buildConfiguration, buildPlatform, compiler.GetCompilerPath(), isRebuild )

# Compile shaders
def _CompileShaders( context ):
    makefiles               = context.get( "makefiles" )
    backend                 = context.get( "backend" )
    buildDir                = context.get( "build_dir" )
    shaderCompilerExe       = f"{context.get( "sdk_bin_dir" )}/{SDK_SHADERCOMPILER_EXE}"
    for makefile in makefiles:
        print( "" )
        print( f"> Compile: {makefile}" )
        print( "", flush=True )
        subprocess.run( [shaderCompilerExe, f"-basedir {buildDir} -makefile {makefile} -backend {backend}"], check=True )

# Compile content
def _CompileContent( context ):
    resourcelists           = context.get( "resourcelists" )
    buildDir                = context.get( "build_dir" )
    resourceCompilerExe     = f"{context.get( "sdk_bin_dir" )}/{SDK_RESOURCECOMPILER_EXE}"
    for resourcelist in resourcelists:
        print( "" )
        print( f"> Compile: {resourcelist}" )
        print( "", flush=True )
        subprocess.run( [resourceCompilerExe, f"-basedir {buildDir} -filelist {resourcelist}"], check=True )

# Generate helper C++ classes for shaders
def _GenerateShaderCppClasses( context ):
    makefiles                   = context.get( "makefiles" )
    repoRoot                    = context.get( "repo_root" )
    useDeployIntermediantDir    = context.get( "use_deploy_intermediant_dir" )
    shaderCompilerExe           = f"{context.get( "sdk_bin_dir" )}/{SDK_SHADERCOMPILER_EXE}"
    intermediateDir             = f"{INTERMEDIATE_AUTOMATIONTOOL_DIR}/{PREMAKE5_ACTION}/" if useDeployIntermediantDir else f"{INTERMEDIATE_ROOT_DIR}/{PREMAKE5_ACTION}/"
    for makefile, outputDir in makefiles:
        print( "" )
        print( f"> Generate C++ Classes: {makefile}" )
        print( "", flush=True )
        subprocess.run( [shaderCompilerExe, f"-makefile {makefile} -gencpp -outcpp {repoRoot}/src/{intermediateDir}/generated/shadercompiler/{outputDir}/ -skipcompilation"], check=True )

# Compile ECS
def _CompileEcs( context ):
    ecsDirs                     = context.get( "ecs_dirs" )
    repoRoot                    = context.get( "repo_root" )
    useDeployIntermediantDir    = context.get( "use_deploy_intermediant_dir" )
    ecsCompilerExe              = f"{context.get( "sdk_bin_dir" )}/{SDK_ECSCOMPILER_EXE}"
    intermediateDir             = f"{INTERMEDIATE_AUTOMATIONTOOL_DIR}/{PREMAKE5_ACTION}/" if useDeployIntermediantDir else f"{INTERMEDIATE_ROOT_DIR}/{PREMAKE5_ACTION}/"
    for ecsDir, outputDir in ecsDirs:
        print( "" )
        print( f"> Compile ECS: {ecsDir}" )
        print( "", flush=True )
        subprocess.run( [ecsCompilerExe, f"-dir {ecsDir} -output {repoRoot}/src/{intermediateDir}/generated/ecs/{outputDir}/"], check=True )

# Copy third party files
def _CopyThirdPartyFiles( context ):
    thirdParty          = context.get( "third_party" )
    buildDir            = context.get( "build_dir" )
    isCopyTools         = context.get( "is_copy_tools" )
    gameDir             = context.get( "game_dir" )
    thirdParty.CopyFiles( buildDir=buildDir, isCopyTools=isCopyTools, gameDir=gameDir )

# Make a file with legal notices
def _MakeLegalNoticesFile( context ):
    thirdParty          = context.get( "third_party" )
    buildDir            = context.get( "build_dir" )
    isWithTools         = context.get( "with_tools" )
    thirdParty.MakeLegalNoticesFile( buildDir=buildDir, isWithTools=isWithTools )


# Builder class
class AutomationTool:
    # Initialize a builder
    def __init__( self, repoRoot, buildPlatform=utils.GetNativePlatform(), buildConfiguration=utils.Configuration.DEBUG, isRebuild=False, sdkDir=None, buildDir=None ):
        self.repoRoot               = os.path.abspath( repoRoot )
        self.buildConfiguration     = buildConfiguration
        self.buildPlatform          = buildPlatform
        self.buildDir               = f"{self.repoRoot}/game/"
        self.sdkDir                 = f"{self.repoRoot}/game/"
        self.thirdParty             = thirdparty.ThirdParty( repoRoot=self.repoRoot, buildPlatform=self.buildPlatform, buildConfiguration=self.buildConfiguration )
        self.isRebuild              = isRebuild
        self._steps                 = []
        if buildDir != None:
            self.buildDir           = os.path.abspath( buildDir )
        if sdkDir != None:
            self.sdkDir             = os.path.abspath( sdkDir )   
        self.sdkBinDir              = f"{self.sdkDir}/bin/" + f"{utils.GetNativePlatform().value}_{utils.Configuration.RELEASE.value}/".lower()

        if not os.path.isfile( f"{self.sdkBinDir}/{SDK_SHADERCOMPILER_EXE}" ) or not os.path.isfile( f"{self.sdkBinDir}/{SDK_RESOURCECOMPILER_EXE}" ) or not os.path.isfile( f"{self.sdkBinDir}/{SDK_ECSCOMPILER_EXE}" ):
            if os.path.isdir( self.sdkDir ) and os.path.samefile( self.sdkDir, f"{self.repoRoot}/game/" ):
                self._AddStep_BuildSDK()
            else:
                raise RuntimeError( f"Invalid Singularity SDK: Shader or resource compiler not found in '{self.sdkDir}'" )

    # Add a custom build step
    def AddStep_Custom( self, step ):
        self._steps.append( step )

    # Add a delete build directory step
    def AddStep_DeleteBuildDir( self ):
        self.AddStep_Custom( step.Step( "Delete Build Directory", _DeleteBuildDir, repo_root=self.repoRoot, build_dir=self.buildDir ) )

    # Add a generate project files step
    def AddStep_GenerateProjectFiles( self, gameName=None ):
        self.AddStep_Custom( step.Step( "Generate Project Files", _GenerateProjectFiles, build_dir=self.buildDir, game=gameName ) )

    # Add a compile projects
    def AddStep_CompileProjects( self, name, projects ):
        self.AddStep_Custom( step.Step( name, _CompileProjects, 
                                       projects=projects, 
                                       repo_root=self.repoRoot, 
                                       build_configuration=self.buildConfiguration, 
                                       build_platform=self.buildPlatform,
                                       is_rebuild=self.isRebuild ) )

    # Compile shaders
    def AddStep_CompileShaders( self, name, makefiles, shaderCompileBackend ):
        _makefiles = []
        for makefile in makefiles:
            _makefiles.append( f"{self.repoRoot}/{makefile}" )
        
        self.AddStep_Custom( step.Step( name, _CompileShaders, 
                                       makefiles=_makefiles,
                                       build_dir=self.buildDir,
                                       sdk_bin_dir=self.sdkBinDir, 
                                       backend=shaderCompileBackend ) )

    # Compile content
    def AddStep_CompileContent( self, name, resourcelists ):
        _resourcelists = []
        for resourcelist in resourcelists:
            _resourcelists.append( f"{self.repoRoot}/{resourcelist}" )

        self.AddStep_Custom( step.Step( name, _CompileContent, resourcelists=_resourcelists, build_dir=self.buildDir, sdk_bin_dir=self.sdkBinDir ) )

    # Add internal steps to build SDK
    def _AddStep_BuildSDK( self ):
        self.AddStep_Custom( step.Step( "[AutomationTool] Generate Project Files (Singularity SDK)", _GenerateProjectFiles, build_dir=self.sdkDir ) )
        self.AddStep_Custom( step.Step( "[AutomationTool] Build Singularity SDK", _CompileProjects, 
                                       projects=TOOL_PROJECTS, 
                                       repo_root=self.repoRoot, 
                                       build_configuration=utils.Configuration.RELEASE, 
                                       build_platform=utils.GetNativePlatform(),
                                       is_rebuild=self.isRebuild ) )
        
        self.sdkThirdParty             = thirdparty.ThirdParty( repoRoot=self.repoRoot, buildPlatform=utils.GetNativePlatform(), buildConfiguration=utils.Configuration.RELEASE )
        self.AddStep_Custom( step.Step( "[AutomationTool] Copy Third Party Files", _CopyThirdPartyFiles,
                                       third_party=self.sdkThirdParty,
                                       build_dir=f"{self.repoRoot}/game/",
                                       is_copy_tools=True ) )

    # Copy third party files
    def AddStep_CopyThirdPartyFiles( self, name, isCopyTools=False, gameDir=None ):
        self.AddStep_Custom( step.Step( name, _CopyThirdPartyFiles,
                                       third_party=self.thirdParty,
                                       build_dir=self.buildDir,
                                       is_copy_tools=isCopyTools,
                                       game_dir=gameDir ) )

    # Make a file with legal notices
    def AddStep_MakeLegalNoticesFile( self, name, isWithTools=False ):
        self.AddStep_Custom( step.Step( name, _MakeLegalNoticesFile, third_party=self.thirdParty, build_dir=self.buildDir, with_tools=isWithTools ) )

    # Add generate C++ classes for step
    def AddStep_GenerateShaderCppClasses( self, name, makefiles, useDeployIntermediantDir=True ):
        _makefiles = []
        for makefile, outputDir in makefiles:
            _makefiles.append( ( f"{self.repoRoot}/{makefile}", outputDir ) )

        self.AddStep_Custom( step.Step( name, _GenerateShaderCppClasses, makefiles=_makefiles, repo_root=self.repoRoot, sdk_bin_dir=self.sdkBinDir, use_deploy_intermediant_dir=useDeployIntermediantDir ) )

    # Add compile a ECS file
    def AddStep_CompileEcs( self, name, ecsDirs, useDeployIntermediantDir=True ):
        _ecsDirs = []
        for ecsDir, outputDir in ecsDirs:
            _ecsDirs.append( ( f"{self.repoRoot}/{ecsDir}", outputDir ) )
        
        self.AddStep_Custom( step.Step( name, _CompileEcs, ecs_dirs=_ecsDirs, repo_root=self.repoRoot, sdk_bin_dir=self.sdkBinDir, use_deploy_intermediant_dir=useDeployIntermediantDir ) )

    # Execute the builder
    def Execute( self ):
        numSteps = len( self._steps )
        try:
            for stepIdx, step in enumerate( self._steps, start=1 ):
                print( "" )
                print( "-------------------------------------------" )
                print( f"{stepIdx}/{numSteps}: {step.name}" )
                print( "-------------------------------------------" )
                print( "", flush=True )
                step.Execute()
        except Exception as exception:
            print( f"Exception: {exception}", flush=True ) 
            traceback.print_exc()
            sys.exit( 1 )