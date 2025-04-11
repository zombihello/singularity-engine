/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef IMATERIALVAR_H
#define IMATERIALVAR_H

#include "core/types.h"
#include "stdlib/math/math.h"

// Forward declarations
class IMaterial;
class ITexture;

/**
 * @ingroup materialsystem
 * @brief Material variable type
 */
enum materialVarType_t
{
	MATERIALVAR_TYPE_UNDEFINED,		/**< Undefined type */
	MATERIALVAR_TYPE_BOOL,			/**< Bool */
	MATERIALVAR_TYPE_INT,			/**< Integer */
	MATERIALVAR_TYPE_FLOAT,			/**< Float */
	MATERIALVAR_TYPE_VECTOR_2D,		/**< Vector 2D */
	MATERIALVAR_TYPE_VECTOR_3D,		/**< Vector 3D */
	MATERIALVAR_TYPE_VECTOR_4D,		/**< Vector 4D */
	MATERIALVAR_TYPE_MATRIX,		/**< Matrix */
	MATERIALVAR_TYPE_STRING,		/**< String */
	MATERIALVAR_TYPE_TEXTURE,		/**< Texture */
	MATERIALVAR_TYPE_MATERIAL		/**< Material */
};

/**
 * @ingroup materialsystem
 * @brief Material variable interface
 */
class IMaterialVar
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IMaterialVar() {}

	/**
	 * @brief Set bool value
	 * @param value		Value
	 */
	virtual void SetBoolValue( bool bValue ) = 0;

	/**
	 * @brief Get bool value
	 * @return Return bool value
	 */
	virtual bool GetBoolValue() const = 0;

	/**
	 * @brief Set integer value
	 * @param value		Value
	 */
	virtual void SetIntValue( int32 value ) = 0;

	/**
	 * @brief Get integer value
	 * @return Return integer value
	 */
	virtual int32 GetIntValue() const = 0;

	/**
	 * @brief Set float value
	 * @param value		Value
	 */
	virtual void SetFloatValue( float value ) = 0;

	/**
	 * @brief Get float value
	 * @return Return float value
	 */
	virtual float GetFloatValue() const = 0;

	/**
	 * @brief Set vector value
	 * @note Number components can take in range from 2 to 4
	 * @param pValue	Pointer to value
	 * @param numComps	Number of vector components, it can take in range from 2 to 4
	 */
	virtual void SetVecValue( const float* pValue, uint32 numComps ) = 0;

	/**
	 * @brief Set vector value
	 * @param value		Value
	 */
	virtual void SetVecValue( const vec2_t& value ) = 0;

	/**
	 * @brief Set vector value
	 * @param value		Value
	 */
	virtual void SetVecValue( const vec3_t& value ) = 0;

	/**
	 * @brief Set vector value
	 * @param value		Value
	 */
	virtual void SetVecValue( const vec4_t& value ) = 0;

	/**
	 * @brief Get vector value
	 * @param pValue	Pointer to data for output
	 * @param numComps	Number of vector components to output
	 */
	virtual void GetVecValue( float* pValue, uint32 numComps ) const = 0;

	/**
	 * @brief Set matrix value
	 * @param value		Value
	 */
	virtual void SetMatrixValue( const matrix_t& value ) = 0;

	/**
	 * @brief Get matrix value
	 * @return Return matrix value
	 */
	virtual matrix_t GetMatrixValue() const = 0;

	/**
	 * @brief Set string value
	 * @param pValue	Value
	 */
	virtual void SetStringValue( const achar* pValue ) = 0;

	/**
	 * @brief Get string value
	 * @return Return string value
	 */
	virtual const achar* GetStringValue() const = 0;

	/**
	 * @brief Set texture value
	 * @param pValue	Value
	 */
	virtual void SetTextureValue( ITexture* pValue ) = 0;

	/**
	 * @brief Get texture value
	 * @return Return texture value
	 */
	virtual ITexture* GetTextureValue() const = 0;

	/**
	 * @brief Set material value
	 * @param pValue	Value
	 */
	virtual void SetMaterialValue( IMaterial* pValue ) = 0;

	/**
	 * @brief Get material value
	 * @return Return material value
	 */
	virtual IMaterial* GetMaterialValue() const = 0;

	/**
	 * @brief Is variable defined
	 * @return Return TRUE if the variable is defined, otherwise return FALSE
	 */
	virtual bool IsDefined() const = 0;

	/**
	 * @brief Set undefined variable
	 */
	virtual void SetUndefined() = 0;

	/**
	 * @brief Get variable name
	 * @return Return variable name
	 */
	virtual const achar* GetName() const = 0;

	/**
	 * @brief Get variable type
	 * @return Return variable type
	 */
	virtual materialVarType_t GetType() const = 0;

	/**
	 * @brief Get owning material
	 * @return Return owning material
	 */
	virtual IMaterial* GetOwningMaterial() const = 0;
};

#endif // !IMATERIALVAR_H