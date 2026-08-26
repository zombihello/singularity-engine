#pragma once
#include "tier0/assert.h"
#include "tier1/math/rotator.h"
#include "tier1/math/color.h"
#include "tier1/math/transform.h"
#include "schemasystem/schematypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
struct schemaCustomOps_t;

//-----------------------------------------------------------------------------
// A described field of a struct/a class
//-----------------------------------------------------------------------------
class ISchemaField
{
public:
	virtual ~ISchemaField() {}

	// Reads/writes/copies this field's value
	virtual bool ReadValue( void* pObject, ISchemaDataReader* pReader ) const		 = 0;
	virtual bool WriteValue( const void* pObject, ISchemaDataWriter* pWriter ) const = 0;
	virtual void CopyValue( void* pDestObject, const void* pSrcObject ) const		 = 0;

	virtual bool					   HasAllFlags( uint32 flags ) const = 0;
	virtual bool					   HasAnyFlags( uint32 flags ) const = 0;
	virtual const char*				   GetName() const					 = 0;
	virtual const char*				   GetAliasName() const				 = 0;
	virtual schemaFieldType_t		   GetType() const					 = 0;
	virtual uint32					   GetFlags() const					 = 0;  // see `schemaFieldFlag_t`
	virtual size					   GetOffset() const				 = 0;
	virtual size					   GetSize() const					 = 0;
	virtual const ISchemaCompoundType* GetOwnerType() const				 = 0;  // The struct or the class that declared this field
	virtual const ISchemaMetadata*	   GetMetadata() const				 = 0;  // NULL when metadata is compiled out

	// The address of the value itself inside the owning object
	void*		GetValuePtr( void* pObject ) const;
	const void* GetValuePtr( const void* pObject ) const;

	// Narrowing to the interface a field of this type is reached through
	// NOTE: A miss gives NULL rather than a pointer to the wrong thing
	template<class TSchemaFieldType>
	const TSchemaFieldType* As() const;
};

//-----------------------------------------------------------------------------
// Everything stored as a number
//-----------------------------------------------------------------------------
class ISchemaFieldNumeric : public ISchemaField
{
public:
	virtual void   SetAsInt64( void* pObject, int64 value ) const	= 0;
	virtual int64  GetAsInt64( const void* pObject ) const			= 0;
	virtual void   SetAsDouble( void* pObject, double value ) const = 0;
	virtual double GetAsDouble( const void* pObject ) const			= 0;
};

//-----------------------------------------------------------------------------
// Integer and floating point fields
//-----------------------------------------------------------------------------
class ISchemaFieldBool : public ISchemaField
{
public:
	virtual void SetValue( void* pObject, const bool& value ) const = 0;
	virtual bool GetValue( const void* pObject ) const				= 0;
};

class ISchemaFieldInt8 : public ISchemaFieldNumeric
{
public:
	virtual void SetValue( void* pObject, const int8& value ) const = 0;
	virtual int8 GetValue( const void* pObject ) const				= 0;
};

class ISchemaFieldInt16 : public ISchemaFieldNumeric
{
public:
	virtual void  SetValue( void* pObject, const int16& value ) const = 0;
	virtual int16 GetValue( const void* pObject ) const				  = 0;
};

class ISchemaFieldInt32 : public ISchemaFieldNumeric
{
public:
	virtual void  SetValue( void* pObject, const int32& value ) const = 0;
	virtual int32 GetValue( const void* pObject ) const				  = 0;
};

class ISchemaFieldInt64 : public ISchemaFieldNumeric
{
public:
	virtual void  SetValue( void* pObject, const int64& value ) const = 0;
	virtual int64 GetValue( const void* pObject ) const				  = 0;
};

class ISchemaFieldUInt8 : public ISchemaFieldNumeric
{
public:
	virtual void  SetValue( void* pObject, const uint8& value ) const = 0;
	virtual uint8 GetValue( const void* pObject ) const				  = 0;
};

class ISchemaFieldUInt16 : public ISchemaFieldNumeric
{
public:
	virtual void   SetValue( void* pObject, const uint16& value ) const = 0;
	virtual uint16 GetValue( const void* pObject ) const				= 0;
};

class ISchemaFieldUInt32 : public ISchemaFieldNumeric
{
public:
	virtual void   SetValue( void* pObject, const uint32& value ) const = 0;
	virtual uint32 GetValue( const void* pObject ) const				= 0;
};

class ISchemaFieldUInt64 : public ISchemaFieldNumeric
{
public:
	virtual void   SetValue( void* pObject, const uint64& value ) const = 0;
	virtual uint64 GetValue( const void* pObject ) const				= 0;
};

class ISchemaFieldFloat : public ISchemaFieldNumeric
{
public:
	virtual void  SetValue( void* pObject, const float& value ) const = 0;
	virtual float GetValue( const void* pObject ) const				  = 0;
};

class ISchemaFieldDouble : public ISchemaFieldNumeric
{
public:
	virtual void   SetValue( void* pObject, const double& value ) const = 0;
	virtual double GetValue( const void* pObject ) const				= 0;
};

//-----------------------------------------------------------------------------
// Math fields
//-----------------------------------------------------------------------------
class ISchemaFieldVector2 : public ISchemaField
{
public:
	virtual void	  SetValue( void* pObject, const vector2_t& value ) const = 0;
	virtual vector2_t GetValue( const void* pObject ) const					  = 0;
};

class ISchemaFieldVector3 : public ISchemaField
{
public:
	virtual void	  SetValue( void* pObject, const vector3_t& value ) const = 0;
	virtual vector3_t GetValue( const void* pObject ) const					  = 0;
};

class ISchemaFieldVector4 : public ISchemaField
{
public:
	virtual void	  SetValue( void* pObject, const vector4_t& value ) const = 0;
	virtual vector4_t GetValue( const void* pObject ) const					  = 0;
};

class ISchemaFieldVector2i : public ISchemaField
{
public:
	virtual void	   SetValue( void* pObject, const vector2i_t& value ) const = 0;
	virtual vector2i_t GetValue( const void* pObject ) const					= 0;
};

class ISchemaFieldVector3i : public ISchemaField
{
public:
	virtual void	   SetValue( void* pObject, const vector3i_t& value ) const = 0;
	virtual vector3i_t GetValue( const void* pObject ) const					= 0;
};

class ISchemaFieldVector4i : public ISchemaField
{
public:
	virtual void	   SetValue( void* pObject, const vector4i_t& value ) const = 0;
	virtual vector4i_t GetValue( const void* pObject ) const					= 0;
};

class ISchemaFieldMatrix3x3 : public ISchemaField
{
public:
	virtual void		SetValue( void* pObject, const matrix3x3_t& value ) const = 0;
	virtual matrix3x3_t GetValue( const void* pObject ) const					  = 0;
};

class ISchemaFieldMatrix4x4 : public ISchemaField
{
public:
	virtual void		SetValue( void* pObject, const matrix4x4_t& value ) const = 0;
	virtual matrix4x4_t GetValue( const void* pObject ) const					  = 0;
};

class ISchemaFieldMatrix3x4 : public ISchemaField
{
public:
	virtual void		SetValue( void* pObject, const matrix3x4_t& value ) const = 0;
	virtual matrix3x4_t GetValue( const void* pObject ) const					  = 0;
};

class ISchemaFieldMatrix4x3 : public ISchemaField
{
public:
	virtual void		SetValue( void* pObject, const matrix4x3_t& value ) const = 0;
	virtual matrix4x3_t GetValue( const void* pObject ) const					  = 0;
};

class ISchemaFieldQuaternion : public ISchemaField
{
public:
	virtual void		 SetValue( void* pObject, const quaternion_t& value ) const = 0;
	virtual quaternion_t GetValue( const void* pObject ) const						= 0;
};

class ISchemaFieldRotator : public ISchemaField
{
public:
	virtual void	 SetValue( void* pObject, const CRotator& value ) const = 0;
	virtual CRotator GetValue( const void* pObject ) const					= 0;
};

class ISchemaFieldTransform : public ISchemaField
{
public:
	virtual void	   SetValue( void* pObject, const CTransform& value ) const = 0;
	virtual CTransform GetValue( const void* pObject ) const					= 0;
};

class ISchemaFieldColor : public ISchemaField
{
public:
	virtual void   SetValue( void* pObject, const CColor& value ) const = 0;
	virtual CColor GetValue( const void* pObject ) const				= 0;
};

//-----------------------------------------------------------------------------
// A `CStringID`, an `eastl::string` or a `const char*` field
//
// NOTE: A `SCHEMA_FIELD_TYPE_CSTRING` field is READ ONLY through the schema: there
//		 is nobody to own the storage the pointer would have to point at. `SetValue`
//		 on one asserts and does nothing
//-----------------------------------------------------------------------------
class ISchemaFieldString : public ISchemaField
{
public:
	virtual void		SetValue( void* pObject, const char* pValue ) const = 0;
	virtual const char* GetValue( const void* pObject ) const				= 0;
};

//-----------------------------------------------------------------------------
// An enum field
//
// NOTE: The value is reached as a number, the underlying width is whatever
//       `ISchemaEnum::GetUnderlyingType` says
//-----------------------------------------------------------------------------
class ISchemaFieldEnum : public ISchemaFieldNumeric
{
public:
	virtual const ISchemaEnum* GetEnumType() const = 0;
};

//-----------------------------------------------------------------------------
// An embedded struct field
//
// NOTE: There is no value accessor here on purpose - take the address with
//       `ISchemaField::GetValuePtr` and walk the fields of `GetStructType`
//-----------------------------------------------------------------------------
class ISchemaFieldStruct : public ISchemaField
{
public:
	virtual const ISchemaStruct* GetStructType() const = 0;
};

//-----------------------------------------------------------------------------
// A pointer to a schema object
//-----------------------------------------------------------------------------
class ISchemaFieldObject : public ISchemaField
{
public:
	virtual void				SetValue( void* pObject, ISchemaObject* pValue ) const = 0;
	virtual ISchemaObject*		GetValue( const void* pObject ) const				   = 0;
	virtual const ISchemaClass* GetObjectType() const								   = 0;
};

//-----------------------------------------------------------------------------
// A custom field, read and written through operations the owning module supplies
//-----------------------------------------------------------------------------
class ISchemaFieldCustom : public ISchemaField
{
public:
	virtual const schemaCustomOps_t* GetCustomOps() const = 0;
};

//-----------------------------------------------------------------------------
// A described member that holds several values
//
// NOTE: The element is described by an INNER field whose offset is zero. Every
//		 operation on it therefore takes the address of the ELEMENT, not of the
//		 owning object - that address is what `GetElementPtr` hands back:
//
//			const ISchemaFieldArray* pArray = pField->As<ISchemaFieldArray>();
//			pArray->SetNumElements( pObject, 8 );
//			pArray->GetInnerField()->As<ISchemaFieldInt32>()->SetValue( pArray->GetElementPtr( pObject, 2 ), 250 );
//-----------------------------------------------------------------------------
class ISchemaFieldContainer : public ISchemaField
{
public:
	virtual const ISchemaField* GetInnerField() const							   = 0;
	virtual uint32				GetNumElements( const void* pObject ) const		   = 0;
	virtual void*				GetElementPtr( void* pObject, uint32 index ) const = 0;	 // NULL when the index is out of range
	const void*					GetElementPtr( const void* pObject, uint32 index ) const;
};

//-----------------------------------------------------------------------------
// `TType[N]` - the count is part of the type, so there is no way to change it
//-----------------------------------------------------------------------------
class ISchemaFieldFixedArray : public ISchemaFieldContainer
{
public:
	virtual uint32 GetNumFixedElements() const = 0;
};

//-----------------------------------------------------------------------------
// `eastl::vector<TType>` - the only field that grows
//-----------------------------------------------------------------------------
class ISchemaFieldArray : public ISchemaFieldContainer
{
public:
	virtual bool InsertElement( void* pObject, uint32 index ) const	 = 0;
	virtual bool RemoveElement( void* pObject, uint32 index ) const	 = 0;
	virtual void ClearElements( void* pObject ) const				 = 0;
	virtual bool SetNumElements( void* pObject, uint32 count ) const = 0;
};

//-----------------------------------------------------------------------------
// Which field types answer for which interface
//-----------------------------------------------------------------------------
template<class TSchemaFieldType>
struct schemaFieldNarrow_t;

#define SCHEMA_DECLARE_FIELD_NARROW( Interface, FirstType, LastType ) \
	template<>                                                        \
	struct schemaFieldNarrow_t<Interface>                             \
	{                                                                 \
		enum                                                          \
		{                                                             \
			FIRST_TYPE = FirstType,                                   \
			LAST_TYPE  = LastType                                     \
		};                                                            \
	}

// Number fields, an enum included
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldNumeric, SCHEMA_FIELD_TYPE_INT8, SCHEMA_FIELD_TYPE_ENUM );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldBool, SCHEMA_FIELD_TYPE_BOOL, SCHEMA_FIELD_TYPE_BOOL );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldInt8, SCHEMA_FIELD_TYPE_INT8, SCHEMA_FIELD_TYPE_INT8 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldInt16, SCHEMA_FIELD_TYPE_INT16, SCHEMA_FIELD_TYPE_INT16 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldInt32, SCHEMA_FIELD_TYPE_INT32, SCHEMA_FIELD_TYPE_INT32 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldInt64, SCHEMA_FIELD_TYPE_INT64, SCHEMA_FIELD_TYPE_INT64 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldUInt8, SCHEMA_FIELD_TYPE_UINT8, SCHEMA_FIELD_TYPE_UINT8 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldUInt16, SCHEMA_FIELD_TYPE_UINT16, SCHEMA_FIELD_TYPE_UINT16 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldUInt32, SCHEMA_FIELD_TYPE_UINT32, SCHEMA_FIELD_TYPE_UINT32 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldUInt64, SCHEMA_FIELD_TYPE_UINT64, SCHEMA_FIELD_TYPE_UINT64 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldFloat, SCHEMA_FIELD_TYPE_FLOAT, SCHEMA_FIELD_TYPE_FLOAT );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldDouble, SCHEMA_FIELD_TYPE_DOUBLE, SCHEMA_FIELD_TYPE_DOUBLE );

// Math fields
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldVector2, SCHEMA_FIELD_TYPE_VECTOR2, SCHEMA_FIELD_TYPE_VECTOR2 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldVector3, SCHEMA_FIELD_TYPE_VECTOR3, SCHEMA_FIELD_TYPE_VECTOR3 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldVector4, SCHEMA_FIELD_TYPE_VECTOR4, SCHEMA_FIELD_TYPE_VECTOR4 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldVector2i, SCHEMA_FIELD_TYPE_VECTOR2I, SCHEMA_FIELD_TYPE_VECTOR2I );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldVector3i, SCHEMA_FIELD_TYPE_VECTOR3I, SCHEMA_FIELD_TYPE_VECTOR3I );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldVector4i, SCHEMA_FIELD_TYPE_VECTOR4I, SCHEMA_FIELD_TYPE_VECTOR4I );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldMatrix3x3, SCHEMA_FIELD_TYPE_MATRIX3X3, SCHEMA_FIELD_TYPE_MATRIX3X3 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldMatrix4x4, SCHEMA_FIELD_TYPE_MATRIX4X4, SCHEMA_FIELD_TYPE_MATRIX4X4 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldMatrix3x4, SCHEMA_FIELD_TYPE_MATRIX3X4, SCHEMA_FIELD_TYPE_MATRIX3X4 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldMatrix4x3, SCHEMA_FIELD_TYPE_MATRIX4X3, SCHEMA_FIELD_TYPE_MATRIX4X3 );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldQuaternion, SCHEMA_FIELD_TYPE_QUATERNION, SCHEMA_FIELD_TYPE_QUATERNION );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldRotator, SCHEMA_FIELD_TYPE_ROTATOR, SCHEMA_FIELD_TYPE_ROTATOR );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldTransform, SCHEMA_FIELD_TYPE_TRANSFORM, SCHEMA_FIELD_TYPE_TRANSFORM );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldColor, SCHEMA_FIELD_TYPE_COLOR, SCHEMA_FIELD_TYPE_COLOR );

// String fields (all three string types share one interface)
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldString, SCHEMA_FIELD_TYPE_STRINGID, SCHEMA_FIELD_TYPE_CSTRING );

// Other fields
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldEnum, SCHEMA_FIELD_TYPE_ENUM, SCHEMA_FIELD_TYPE_ENUM );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldStruct, SCHEMA_FIELD_TYPE_STRUCT, SCHEMA_FIELD_TYPE_STRUCT );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldObject, SCHEMA_FIELD_TYPE_OBJECTPTR, SCHEMA_FIELD_TYPE_OBJECTPTR );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldCustom, SCHEMA_FIELD_TYPE_CUSTOM, SCHEMA_FIELD_TYPE_CUSTOM );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldFixedArray, SCHEMA_FIELD_TYPE_FIXED_ARRAY, SCHEMA_FIELD_TYPE_FIXED_ARRAY );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldArray, SCHEMA_FIELD_TYPE_ARRAY, SCHEMA_FIELD_TYPE_ARRAY );
SCHEMA_DECLARE_FIELD_NARROW( ISchemaFieldContainer, SCHEMA_FIELD_TYPE_FIXED_ARRAY, SCHEMA_FIELD_TYPE_ARRAY );
#undef SCHEMA_DECLARE_FIELD_NARROW

#include "schemasystem/ischemafield.inl"
