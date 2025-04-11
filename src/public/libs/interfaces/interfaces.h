/**
 * @file
 * @addtogroup interfaces interfaces
 */

#ifndef INTERFACES_H
#define INTERFACES_H

// Forward declarations
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

/**
 * @ingroup interfaces
 * @brief File system
 */
extern IFileSystem* g_pFileSystem;

/**
 * @ingroup interfaces
 * @brief Window manager
 */
extern IWindowMgr* g_pWindowMgr;

/**
 * @ingroup interfaces
 * @brief Input system
 */
extern IInputSystem* g_pInputSystem;

/**
 * @ingroup interfaces
 * @brief Console system
 */
extern ICvar* g_pCvar;

/**
 * @ingroup interfaces
 * @brief Game
 */
extern IGame* g_pGame;

/**
 * @ingroup interfaces
 * @brief Studio render
 */
extern IStudioRender* g_pStudioRender;

/**
 * @ingroup interfaces
 * @brief Studio API
 */
extern IStudioAPI* g_pStudioAPI;

/**
 * @ingroup interfaces
 * @brief Shader manager
 */
extern IShaderMgr* g_pShaderMgr;

/**
 * @ingroup interfaces
 * @brief Material system
 */
extern IMaterialSystem* g_pMaterialSystem;

/**
 * @ingroup interfaces
 * @brief Resource system
 */
extern IResourceSystem* g_pResourceSystem;

#endif // !INTERFACES_H