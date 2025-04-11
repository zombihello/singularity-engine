/**
 * @file
 * @addtogroup smatdoc smatdoc
 */

#ifndef SMATVAR_H
#define SMATVAR_H

#include <string>

#include "core/core.h"
#include "core/debug.h"
#include "stdlib/math/math.h"

/**
 * @ingroup smatdoc
 * @brief SMAT material variable type
 */
enum smatMaterialVarType_t
{
	SMAT_MATERIAL_VAR_TYPE_UNDEFINED,		/**< Undefined */
	SMAT_MATERIAL_VAR_TYPE_BOOL,			/**< Bool */
	SMAT_MATERIAL_VAR_TYPE_INT,				/**< Integer */
	SMAT_MATERIAL_VAR_TYPE_FLOAT,			/**< Float */
	SMAT_MATERIAL_VAR_TYPE_VECTOR_2D,		/**< Vector 2D */
	SMAT_MATERIAL_VAR_TYPE_VECTOR_3D,		/**< Vector 3D */
	SMAT_MATERIAL_VAR_TYPE_VECTOR_4D,		/**< Vector 4D */
	SMAT_MATERIAL_VAR_TYPE_MATRIX,			/**< Matrix */
	SMAT_MATERIAL_VAR_TYPE_STRING,			/**< String */
	SMAT_MATERIAL_VAR_TYPE_TEXTURE,			/**< Texture */
	SMAT_MATERIAL_VAR_TYPE_MATERIAL			/**< Material */
};

/**
 * @ingroup smatdoc
 * @brief SMAT material variable
 */
class CSMATMaterialVar
{
public:
	/**
	 * @brief Constructor
	 */
	CSMATMaterialVar();

	/**
	 * @brief Constructor
	 * @param other		Other SMAT material variable
	 */
	CSMATMaterialVar( const CSMATMaterialVar& other );

	/**
	 * @brief Destructor
	 */
	~CSMATMaterialVar();

	/**
	 * @brief Copy material variable
	 * @param other		Other SMAT material variable
	 */
	void Copy( const CSMATMaterialVar& other );

	/**
	 * @brief Set name
	 * @param pName		Variable name
	 */
	FORCEINLINE void SetName( const achar* pName )
	{
		name = pName;
	}

	/**
	 * @brief Set bool value
	 * @param value		Value
	 */
	FORCEINLINE void SetBoolValue( bool bValue )
	{
		boolValue	= bValue;
		type		= SMAT_MATERIAL_VAR_TYPE_BOOL;
	}

	/**
	 * @brief Get bool value
	 * @return Return bool value
	 */
	FORCEINLINE bool GetBoolValue() const
	{
		switch ( type )
		{
		case SMAT_MATERIAL_VAR_TYPE_BOOL:	return boolValue;
		case SMAT_MATERIAL_VAR_TYPE_INT:	return intValue > 0;
		case SMAT_MATERIAL_VAR_TYPE_FLOAT:	return floatValue > 0.f;
		default:							return false;
		}
	}

	/**
	 * @brief Set integer value
	 * @param value		Value
	 */
	FORCEINLINE void SetIntValue( int32 value )
	{
		intValue	= value;
		type		= SMAT_MATERIAL_VAR_TYPE_INT;
	}

	/**
	 * @brief Get integer value
	 * @return Return integer value
	 */
	FORCEINLINE int32 GetIntValue() const
	{
		switch ( type )
		{
		case SMAT_MATERIAL_VAR_TYPE_BOOL:	return ( int32 )boolValue;
		case SMAT_MATERIAL_VAR_TYPE_INT:	return intValue;
		case SMAT_MATERIAL_VAR_TYPE_FLOAT:	return ( int32 )floatValue;
		default:							return 0;
		}
	}

	/**
	 * @brief Set float value
	 * @param value		Value
	 */
	FORCEINLINE void SetFloatValue( float value )
	{
		floatValue	= value;
		type		= SMAT_MATERIAL_VAR_TYPE_FLOAT;
	}

	/**
	 * @brief Get float value
	 * @return Return float value
	 */
	FORCEINLINE float GetFloatValue() const
	{
		switch ( type )
		{
		case SMAT_MATERIAL_VAR_TYPE_BOOL:	return ( float )boolValue;
		case SMAT_MATERIAL_VAR_TYPE_INT:	return ( float )intValue;
		case SMAT_MATERIAL_VAR_TYPE_FLOAT:	return floatValue;
		default:							return 0.f;
		}
	}

	/**
	 * @brief Set vector value
	 * @note Number components can take in range from 2 to 4
	 * @param pValue	Pointer to value
	 * @param numComps	Number of vector components, it can take in range from 2 to 4
	 */
	FORCEINLINE void SetVecValue( const float* pValue, uint32 numComps )
	{
		switch ( numComps )
		{
		case 2: SetVecValue( *( vec2_t* )pValue ); break;
		case 3: SetVecValue( *( vec3_t* )pValue ); break;
		case 4: SetVecValue( *( vec4_t* )pValue ); break;
		default:
			AssertMsg( false, "A material variable can take only in range from 2 to 4" );
			break;
		}
	}

	/**
	 * @brief Set vector value
	 * @param value		Value
	 */
	FORCEINLINE void SetVecValue( const vec2_t& value )
	{
		vector2DValue	= value;
		type			= SMAT_MATERIAL_VAR_TYPE_VECTOR_2D;
	}

	/**
	 * @brief Set vector value
	 * @param value		Value
	 */
	FORCEINLINE void SetVecValue( const vec3_t& value )
	{
		vector3DValue	= value;
		type			= SMAT_MATERIAL_VAR_TYPE_VECTOR_3D;
	}

	/**
	 * @brief Set vector value
	 * @param value		Value
	 */
	FORCEINLINE void SetVecValue( const vec4_t& value )
	{
		vector4DValue	= value;
		type			= SMAT_MATERIAL_VAR_TYPE_VECTOR_4D;
	}

	/**
	 * @brief Get vector value
	 * @param pValue	Pointer to data for output
	 * @param numComps	Number of vector components to output
	 */
	FORCEINLINE void GetVecValue( float* pValue, uint32 numComps ) const
	{
		Assert( numComps >= 2 && numComps <= 4 );
		switch ( type )
		{
		case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
			Assert( numComps >= 2 );
			Mem_Memcpy( pValue, &vector2DValue, numComps * sizeof( float ) );
			break;

		case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
			Assert( numComps >= 3 );
			Mem_Memcpy( pValue, &vector3DValue, numComps * sizeof( float ) );
			break;

		case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
			Assert( numComps >= 4 );
			Mem_Memcpy( pValue, &vector4DValue, numComps * sizeof( float ) );
			break;

		default:
			Mem_Memset( pValue, 0, numComps * sizeof( float ) );
			break;
		}
	}

	/**
	 * @brief Set matrix value
	 * @param value		Value
	 */
	FORCEINLINE void SetMatrixValue( const matrix_t& value )
	{
		matrixValue = value;
		type		= SMAT_MATERIAL_VAR_TYPE_MATRIX;
	}

	/**
	 * @brief Get matrix value
	 * @return Return matrix value
	 */
	FORCEINLINE matrix_t GetMatrixValue() const
	{
		return type == SMAT_MATERIAL_VAR_TYPE_MATRIX ? matrixValue : math_t::matrixIdentity;
	}

	/**
	 * @brief Set string value
	 * @param pValue	Value
	 */
	FORCEINLINE void SetStringValue( const achar* pValue )
	{
		stringValue		= pValue;
		pStringValue	= stringValue.c_str();
		type			= SMAT_MATERIAL_VAR_TYPE_STRING;
	}

	/**
	 * @brief Get string value
	 * @return Return string value
	 */
	FORCEINLINE const achar* GetStringValue() const
	{
		return type == SMAT_MATERIAL_VAR_TYPE_STRING ? pStringValue : "";
	}

	/**
	 * @brief Set texture value
	 * @param pValue	Value
	 */
	FORCEINLINE void SetTextureValue( const achar* pValue )
	{
		stringValue		= pValue;
		pTextureValue	= stringValue.c_str();
		type			= SMAT_MATERIAL_VAR_TYPE_TEXTURE;
	}

	/**
	 * @brief Get texture value
	 * @return Return texture value
	 */
	FORCEINLINE const achar* GetTextureValue() const
	{
		return type == SMAT_MATERIAL_VAR_TYPE_TEXTURE ? pTextureValue : "";
	}

	/**
	 * @brief Set material value
	 * @param pValue	Value
	 */
	FORCEINLINE void SetMaterialValue( const achar* pValue )
	{
		stringValue		= pValue;
		pMaterialValue	= stringValue.c_str();
		type			= SMAT_MATERIAL_VAR_TYPE_MATERIAL;
	}

	/**
	 * @brief Get material value
	 * @return Return material value
	 */
	FORCEINLINE const achar* GetMaterialValue() const
	{
		return type == SMAT_MATERIAL_VAR_TYPE_MATERIAL ? pMaterialValue : "";
	}

	/**
	 * @brief Is variable defined
	 * @return Return TRUE if the variable is defined, otherwise return FALSE
	 */
	FORCEINLINE bool IsDefined() const
	{
		return type != SMAT_MATERIAL_VAR_TYPE_UNDEFINED;
	}

	/**
	 * @brief Set undefined variable
	 */
	FORCEINLINE void SetUndefined()
	{
		type = SMAT_MATERIAL_VAR_TYPE_UNDEFINED;
	}

	/**
	 * @brief Get variable name
	 * @return Return variable name
	 */
	FORCEINLINE const achar* GetName() const
	{
		return name.c_str();
	}

	/**
	 * @brief Get variable type
	 * @return Return variable type
	 */
	FORCEINLINE smatMaterialVarType_t GetType() const
	{
		return type;
	}

	/**
	 * @brief Override of operator copy
	 * @param right		SMAT copy
	 * @return Return reference to self
	 */
	FORCEINLINE CSMATMaterialVar& operator=( const CSMATMaterialVar& right )
	{
		Copy( right );
		return *this;
	}

private:
	std::string				name;				/**< Variable name */
	smatMaterialVarType_t	type;				/**< Variable type */
	std::string				stringValue;		/**< String value */
	union
	{
		bool				boolValue;			/**< Bool value */
		int32				intValue;			/**< Integer value */
		float				floatValue;			/**< Float value */
		vec2_t				vector2DValue;		/**< Vector 2D value */
		vec3_t				vector3DValue;		/**< Vector 3D value */
		vec4_t				vector4DValue;		/**< Vector 4D value */
		matrix_t			matrixValue;		/**< Matrix value */
		const achar*		pStringValue;		/**< String value */
		const achar*		pTextureValue;		/**< Texture value */
		const achar*		pMaterialValue;		/**< Material value */
	};
};

#endif // !SMATVAR_H