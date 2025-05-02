#ifndef SENTVAR_H
#define SENTVAR_H

#include "core/core.h"
#include "core/debug.h"
#include "stdlib/math/math.h"

//-----------------------------------------------------------------------------
// SENT entity descriptor variable
//-----------------------------------------------------------------------------
enum sentEntityDescVarType_t
{
	SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED,
	SENT_ENTITY_DESC_VAR_TYPE_BOOL,
	SENT_ENTITY_DESC_VAR_TYPE_INT,
	SENT_ENTITY_DESC_VAR_TYPE_FLOAT,
	SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D,
	SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D,
	SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D,
	SENT_ENTITY_DESC_VAR_TYPE_MATRIX,
	SENT_ENTITY_DESC_VAR_TYPE_STRING
};


class CSENTEntityDescVar
{
public:
	CSENTEntityDescVar();
	CSENTEntityDescVar( const CSENTEntityDescVar& other );
	~CSENTEntityDescVar();

	void Copy( const CSENTEntityDescVar& other );

	void SetName( const achar* pName );
	void SetUndefined();
	void SetBoolValue( bool bValue );
	void SetIntValue( int32 value );
	void SetFloatValue( float value );
	void SetVecValue( const float* pValue, uint32 numComps );
	void SetVecValue( const vec2_t& value );
	void SetVecValue( const vec3_t& value );
	void SetVecValue( const vec4_t& value );
	void SetMatrixValue( const matrix_t& value );
	void SetStringValue( const achar* pValue );

	bool IsDefined() const;
	const achar* GetName() const;
	sentEntityDescVarType_t GetType() const;
	bool GetBoolValue() const;
	int32 GetIntValue() const;
	float GetFloatValue() const;
	void GetVecValue( float* pValue, uint32 numComps ) const;
	matrix_t GetMatrixValue() const;
	const achar* GetStringValue() const;

	CSENTEntityDescVar& operator=( const CSENTEntityDescVar& right );

private:
	std::string					name;
	sentEntityDescVarType_t		type;
	std::string					stringValue;
	union
	{
		bool					boolValue;
		int32					intValue;
		float					floatValue;
		vec2_t					vector2DValue;
		vec3_t					vector3DValue;
		vec4_t					vector4DValue;
		matrix_t				matrixValue;
		const achar*			pStringValue;
	};
};

#include "sentdoc/sentvar.inl"

#endif // !SENTVAR_H