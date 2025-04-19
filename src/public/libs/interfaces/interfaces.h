/**
 * @file
 * @addtogroup interfaces interfaces
 */

#ifndef INTERFACES_H
#define INTERFACES_H

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IFileSystem;
class IWindowMgr;
class IInputSystem;
class ICvar;
class IGame;
class IStudioRender;
class IStudioAPI;
class IShaderMgr;
class IMaterialSystem;
class IResourceSystem;


//-----------------------------------------------------------------------------
// Global pointers to app systems
//-----------------------------------------------------------------------------
extern IFileSystem*		g_pFileSystem;
extern IWindowMgr*		g_pWindowMgr;
extern IInputSystem*	g_pInputSystem;
extern ICvar*			g_pCvar;
extern IGame*			g_pGame;
extern IStudioRender*	g_pStudioRender;
extern IStudioAPI*		g_pStudioAPI;
extern IShaderMgr*		g_pShaderMgr;
extern IMaterialSystem* g_pMaterialSystem;
extern IResourceSystem* g_pResourceSystem;

#endif // !INTERFACES_H