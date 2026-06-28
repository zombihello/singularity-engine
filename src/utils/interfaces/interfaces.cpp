#include "tier0/tier0.h"
#include "utils/interfaces/interfaces.h"

IFileSystem*	 g_pFileSystem	   = NULL;
IWindowMgr*		 g_pWindowMgr	   = NULL;
IInputSystem*	 g_pInputSystem	   = NULL;
ICmdSystem*		 g_pCmdSystem	   = NULL;
ICVarSystem*	 g_pCVarSystem	   = NULL;
IGame*			 g_pGame		   = NULL;
IStudioRender*	 g_pStudioRender   = NULL;
IStudioAPI*		 g_pStudioAPI	   = NULL;
IShaderMgr*		 g_pShaderMgr	   = NULL;
IMaterialSystem* g_pMaterialSystem = NULL;
IResourceSystem* g_pResourceSystem = NULL;
IModelSystem*	 g_pModelSystem	   = NULL;