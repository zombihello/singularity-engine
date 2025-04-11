/**
 * @file
 * @addtogroup shaderlib shaderlib
 */

#ifndef SHADERLIB_H
#define SHADERLIB_H

#include <vector>

#include "materialsystem/ishadermgr.h"
#include "shaderlib/shader_base.h"

/**
 * @ingroup shaderslibrary
 * @brief Shaders library
 */
class CShaderLib : public IShaderLib
{
public:
	/**
	 * @brief Constructor
	 * @param pName		Shader library name
	 */
	CShaderLib( const achar* pName );

	/**
	 * @brief Connect shader library
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes the library is connected, otherwise return FALSE
	 */
	virtual bool Connect( createInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect shader library
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Initialize the shader library
	 * @param shaderLibIndex	Shader library index in the shader manager
	 */
	virtual void Init( uint32 shaderLibIndex ) override;

	/**
	 * @brief Shutdown the shader library
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Get shader library name
	 * @return Return shader library name
	 */
	virtual const achar* GetName() const override;

	/**
	 * @brief Get number of shaders in this library
	 * @return Return number of shaders in this library
	 */
	virtual uint32 GetNumShaders() const override;

	/**
	 * @brief Get shader by index
	 *
	 * @param index		Index
	 * @return Return shader at index
	 */
	virtual IShader* GetShader( uint32 index ) const override;

	/**
	 * @brief Get the shader library index
	 * @return Return the shader library index in the shader manager
	 */
	virtual uint32 GetIndex() const override;

	/**
	 * @brief Adds a shader to the list of shaders
	 * @param pShader	shader_t
	 */
	FORCEINLINE void InsertShader( CBaseShader* pShader )
	{
		Assert( pShader );
		shaders.emplace_back( pShader );
	}

private:
	std::string						name;		/**< Name */
	uint32							index;		/**< Index in the shader manager */
	std::vector<CBaseShader*>		shaders;	/**< Shaders */
};

/**
 * @ingroup shaderslibrary
 * @brief Shader library
 */
extern CShaderLib		g_ShaderLib;

#endif // !SHADERLIB_H