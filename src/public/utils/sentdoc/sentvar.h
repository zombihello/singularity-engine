#pragma once
#include "tier0/tier0.h"
#include "tier0/ilogger.h"
#include "tier1/math/math.h"

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
	SENT_ENTITY_DESC_VAR_TYPE_STRING,
	SENT_ENTITY_DESC_VAR_NUM_TYPES

	// TODO BS yehor.pohuliaka - Add support of arrays and dictionaries
};

class CSENTEntityDescVar
{
public:
	CSENTEntityDescVar();
	CSENTEntityDescVar( const CSENTEntityDescVar& other );
	~CSENTEntityDescVar();

	void Copy( const CSENTEntityDescVar& other );

	void SetName( const char* pName );
	void SetUndefined();
	void SetBoolValue( bool bValue );
	void SetIntValue( int32 value );
	void SetFloatValue( float value );
	void SetVec2Value( const vector2_t& value );
	void SetVec3Value( const vector3_t& value );
	void SetVec4Value( const vector4_t& value );
	void SetMatrixValue( const matrix4x4_t& value );
	void SetStringValue( const char* pValue );

	bool					IsDefined() const;
	bool					IsA( sentEntityDescVarType_t type ) const;
	const char*				GetName() const;
	sentEntityDescVarType_t GetType() const;
	bool					GetBoolValue( bool defaultValue = false ) const;
	int32					GetIntValue( int32 defaultValue = 0 ) const;
	float					GetFloatValue( float defaultValue = 0.f ) const;
	vector2_t				GetVec2Value( const vector2_t& defaultValue = vector2_t( 0.f, 0.f ) ) const;
	vector3_t				GetVec3Value( const vector3_t& defaultValue = g_vector000 ) const;
	vector4_t				GetVec4Value( const vector4_t& defaultValue = vector4_t( 0.f, 0.f, 0.f, 0.f ) ) const;
	matrix4x4_t				GetMatrixValue( const matrix4x4_t& defaultValue = g_matrix44Identity ) const;
	const char*				GetStringValue( const char* pDefaultValue = "" ) const;

	CSENTEntityDescVar& operator=( const CSENTEntityDescVar& right );

private:
	eastl::string			name;
	sentEntityDescVarType_t type;
	eastl::string			stringValue;
	union
	{
		bool		boolValue;
		int32		intValue;
		float		floatValue;
		vector2_t	vector2DValue;
		vector3_t	vector3DValue;
		vector4_t	vector4DValue;
		matrix4x4_t matrixValue;
		const char* pStringValue;
	};
};

#include "utils/sentdoc/sentvar.inl"
