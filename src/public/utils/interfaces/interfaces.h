#pragma once

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IFileSystem;
class IWindowMgr;
class IInputSystem;
class ICmdSystem;
class ICVarSystem;
class IGame;
class IStudioRender;
class IStudioAPI;
class IShaderMgr;
class IMaterialSystem;
class IResourceSystem;
class IModelSystem;

//-----------------------------------------------------------------------------
// Global pointers to app systems
//-----------------------------------------------------------------------------
extern IFileSystem*		g_pFileSystem;
extern IWindowMgr*		g_pWindowMgr;
extern IInputSystem*	g_pInputSystem;
extern ICmdSystem*		g_pCmdSystem;
extern ICVarSystem*		g_pCVarSystem;
extern IGame*			g_pGame;
extern IStudioRender*	g_pStudioRender;
extern IStudioAPI*		g_pStudioAPI;
extern IShaderMgr*		g_pShaderMgr;
extern IMaterialSystem* g_pMaterialSystem;
extern IResourceSystem* g_pResourceSystem;
extern IModelSystem*	g_pModelSystem;
