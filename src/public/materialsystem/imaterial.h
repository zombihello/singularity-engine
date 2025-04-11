/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef IMATERIAL_H
#define IMATERIAL_H

#include "stdlib/refcount.h"
#include "studiorender/studio_renderpasstypes.h"

// Forward declarations
class IStudioAPICmdList;
class IMaterialVar;
class IShader;

/**
 * @ingroup materialsystem
 * @brief Material interface
 */
class IMaterial : public IRefCounted
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IMaterial() {}

	/**
	 * @brief Place barriers into a list
	 * @warning This is only called by the render thread
	 *
	 * @param pStudioAPICmdList	StudioAPI command list
	 */
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) = 0;

	/**
	 * @brief Prepare for draw
	 * @warning This is only called by the render thread
	 *
	 * @param pStudioAPICmdList	StudioAPI command list
	 * @param renderPassType	Render pass type
	 */
	virtual void R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType ) = 0;

	/**
	 * @brief Set a shader by name
	 * Find in the shader system a shader by name, set it and add to the material all shader parameters
	 * @param pShaderName	Shader name
	 */
	virtual void SetShader( const achar* pShaderName ) = 0;

	/**
	 * @brief Find material variable
	 * @param pName		Variable name
	 * @return Return founded material variable. If not found returns NULL
	 */
	virtual IMaterialVar* FindVar( const achar* pName ) const = 0;

	/**
	 * @brief Get number of material variables
	 * @return Return number of material variables
	 */
	virtual uint32 GetNumVars() const = 0;

	/**
	 * @brief Get material variables
	 * @return Return array of material variables
	 */
	virtual IMaterialVar** GetVars() const = 0;

	/**
	 * @brief Get shader name
	 * @return Return shader name. If not set return empty string
	 */
	virtual const achar* GetShaderName() const = 0;

	/**
	 * @brief Get the material shader
	 * @return Return the material shader. If not set returns NULL
	 */
	virtual IShader* GetShader() const = 0;
};

#endif // !IMATERIAL_H