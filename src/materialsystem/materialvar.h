/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef MATERIALVAR_H
#define MATERIALVAR_H

#include "materialsystem/imaterialvar.h"

// Forward declarations
class CMaterial;

/**
 * @ingroup materialsystem
 * @brief Material variable
 */
class CMaterialVar : public IMaterialVar
{
public:
	/**
	 * @brief Constructor
	 * @param pMaterial		Owner material
	 * @param pName			Variable name
	 */
	CMaterialVar( IMaterial* pMaterial, const achar* pName );

	/**
	 * @brief Destructor
	 */
	~CMaterialVar();

	/**
	 * @brief Set bool value
	 * @param value		Value
	 */
	virtual void SetBoolValue( bool bValue ) override;

	/**
	 * @brief Get bool value
	 * @return Return bool value
	 */
	virtual bool GetBoolValue() const override;

	/**
	 * @brief Set integer value
	 * @param value		Value
	 */
	virtual void SetIntValue( int32 value ) override;

	/**
	 * @brief Get integer value
	 * @return Return integer value
	 */
	virtual int32 GetIntValue() const override;

	/**
	 * @brief Set float value
	 * @param value		Value
	 */
	virtual void SetFloatValue( float value ) override;

	/**
	 * @brief Get float value
	 * @return Return float value
	 */
	virtual float GetFloatValue() const override;

	/**
	 * @brief Set vector value
	 * @note Number components can take in range from 2 to 4
	 * @param pValue	Pointer to value
	 * @param numComps	Number of vector components, it can take in range from 2 to 4
	 */
	virtual void SetVecValue( const float* pValue, uint32 numComps ) override;

	/**
	 * @brief Set vector value
	 * @param value		Value
	 */
	virtual void SetVecValue( const vec2_t& value ) override;

	/**
	 * @brief Set vector value
	 * @param value		Value
	 */
	virtual void SetVecValue( const vec3_t& value ) override;

	/**
	 * @brief Set vector value
	 * @param value		Value
	 */
	virtual void SetVecValue( const vec4_t& value ) override;

	/**
	 * @brief Get vector value
	 * @param pValue	Pointer to data for output
	 * @param numComps	Number of vector components to output
	 */
	virtual void GetVecValue( float* pValue, uint32 numComps ) const override;

	/**
	 * @brief Set matrix value
	 * @param value		Value
	 */
	virtual void SetMatrixValue( const matrix_t& value ) override;

	/**
	 * @brief Get matrix value
	 * @return Return matrix value
	 */
	virtual matrix_t GetMatrixValue() const override;

	/**
	 * @brief Set string value
	 * @param pValue	Value
	 */
	virtual void SetStringValue( const achar* pValue ) override;

	/**
	 * @brief Get string value
	 * @return Return string value
	 */
	virtual const achar* GetStringValue() const override;

	/**
	 * @brief Set texture value
	 * @param pValue	Value
	 */
	virtual void SetTextureValue( ITexture* pValue ) override;

	/**
	 * @brief Get texture value
	 * @return Return texture value
	 */
	virtual ITexture* GetTextureValue() const override;

	/**
	 * @brief Set material value
	 * @param pValue	Value
	 */
	virtual void SetMaterialValue( IMaterial* pValue ) override;

	/**
	 * @brief Get material value
	 * @return Return material value
	 */
	virtual IMaterial* GetMaterialValue() const override;

	/**
	 * @brief Is variable defined
	 * @return Return TRUE if the variable is defined, otherwise return FALSE
	 */
	virtual bool IsDefined() const override;

	/**
	 * @brief Set undefined variable
	 */
	virtual void SetUndefined() override;

	/**
	 * @brief Get variable name
	 * @return Return variable name
	 */
	virtual const achar* GetName() const override;

	/**
	 * @brief Get variable type
	 * @return Return variable type
	 */
	virtual materialVarType_t GetType() const override;

	/**
	 * @brief Get owning material
	 * @return Return owning material
	 */
	virtual IMaterial* GetOwningMaterial() const override;

private:
	const achar*				pName;				/**< Variable name */
	materialVarType_t			type;				/**< Variable type */
	CMaterial*					pOwningMaterial;	/**< Owning material */
	union
	{
		bool					boolValue;			/**< Bool value */
		int32					intValue;			/**< Integer value */
		float					floatValue;			/**< Float value */
		vec2_t					vector2DValue;		/**< Vector 2D value */
		vec3_t					vector3DValue;		/**< Vector 3D value */
		vec4_t					vector4DValue;		/**< Vector 4D value */
		matrix_t				matrixValue;		/**< Matrix value */
		std::string				stringValue;		/**< String value */
		ITexture*				pTextureValue;		/**< Texture value */
		TRefPtr<CMaterial>		pMaterialValue;		/**< Material value */
	};
};

#endif // !MATERIALVAR_H