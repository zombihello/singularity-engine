#pragma once
#include "schemasystem/ischemametadata.h"
#include "schemasystem/ischematype.h"
#include "schemasystem/schemadesc.h"
#include "schemasystem/schemasystem.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CSchemaTypeBase;
class CSchemaScope;

//-----------------------------------------------------------------------------
// Schema internal definitions
//-----------------------------------------------------------------------------
enum schemaTypeMaskFlags_t
{
	SCHEMA_TYPE_MASK_FLAGS_INHERITED = SCHEMA_TYPE_FLAG_DEPRECATED | SCHEMA_TYPE_FLAG_HIDDEN | SCHEMA_TYPE_FLAG_TRANSIENT | SCHEMA_TYPE_FLAG_REFCOUNTED
};

enum schemaSerializeSkipFlag_t
{
	SCHEMA_SERIALIZE_SKIP_TYPE_FLAGS  = SCHEMA_TYPE_FLAG_TRANSIENT | SCHEMA_TYPE_FLAG_DEPRECATED,
	SCHEMA_SERIALIZE_SKIP_FIELD_FLAGS = SCHEMA_FIELD_FLAG_DEPRECATED
};

enum schemaTypeResolveState_t
{
	SCHEMA_TYPE_RESOLVE_STATE_UNRESOLVED,
	SCHEMA_TYPE_RESOLVE_STATE_RESOLVING,
	SCHEMA_TYPE_RESOLVE_STATE_RESOLVED
};

//-----------------------------------------------------------------------------
// Schema type internal interface
//-----------------------------------------------------------------------------
class ISchemaTypeInternal
{
public:
	template<class TBaseClass>
	friend class CSchemaTypeBase;
	virtual ~ISchemaTypeInternal() {}

	// Settles everything this type needed the OTHER types to exist for. Recursive
	// through the base, and idempotent - the registry resolves in whatever order
	// the modules happened to register
	virtual void Resolve()			= 0;
	virtual void Unresolve()		= 0;
	virtual bool IsResolved() const = 0;

	virtual schemaScopeId_t		 GetScopeId() const					= 0;
	virtual uint32				 GetNumDependents() const			= 0;
	virtual ISchemaTypeInternal* GetDependent( uint32 index ) const = 0;

	virtual ISchemaType* AsISchemaType() = 0;
	const ISchemaType*	 AsISchemaType() const;

private:
	// The scope this type was registered into
	virtual void SetScopeId( schemaScopeId_t scopeId ) = 0;

	// Every type that resolved a pointer AT this one - as its base, as an interface
	// it implements, or as the type of one of its fields
	virtual void AddDependent( ISchemaTypeInternal* pType )	   = 0;
	virtual void RemoveDependent( ISchemaTypeInternal* pType ) = 0;
};

//-----------------------------------------------------------------------------
// Metadata of a described type or field
//-----------------------------------------------------------------------------
#if ENABLE_SCHEMA_METADATA
class CSchemaMetadata : public ISchemaMetadata
{
public:
	// ISchemaMetadata interface
	virtual const char* FindValue( const char* pKey, const char* pDefaultValue = "" ) const override;

	virtual uint32		GetNum() const override;
	virtual const char* GetKey( uint32 index ) const override;
	virtual const char* GetValue( uint32 index ) const override;

	static CSchemaMetadata* Make( const schemaMetadataDesc_t* pMetadata, uint32 numMetadata );

	void Init( const schemaMetadataDesc_t* pMetadata, uint32 numMetadata );
	bool IsEmpty() const;

private:
	using metadataDict_t = eastl::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t>;

	eastl::vector<schemaMetadataDesc_t> metadata;
	metadataDict_t						metadataDict;
};
#endif	// ENABLE_SCHEMA_METADATA

//-----------------------------------------------------------------------------
// Everything a described type does that does not depend on its kind
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CSchemaTypeBase : public TBaseClass, public ISchemaTypeInternal
{
public:
	friend CSchemaScope;
	using ISchemaTypeInternal::AsISchemaType;

	// ISchemaType interface
	virtual bool				   HasAllFlags( uint32 flags ) const override;
	virtual bool				   HasAnyFlags( uint32 flags ) const override;
	virtual schemaTypeKind_t	   GetKind() const override;
	virtual const char*			   GetName() const override;
	virtual const char*			   GetAliasName() const override;
	virtual uint32				   GetFlags() const override;
	virtual size				   GetSize() const override;
	virtual uint32				   GetAlignment() const override;
	virtual const ISchemaMetadata* GetMetadata() const override;
	virtual bool				   IsA( const ISchemaType* pType ) const override;

	// ISchemaTypeInternal interface
	virtual void Resolve() override;
	virtual void Unresolve() override;
	virtual bool IsResolved() const override;

	virtual schemaScopeId_t		 GetScopeId() const override;
	virtual uint32				 GetNumDependents() const override;
	virtual ISchemaTypeInternal* GetDependent( uint32 index ) const override;
	virtual ISchemaType*		 AsISchemaType() override;

	CSchemaTypeBase( schemaTypeKind_t kind, const char* pName, const char* pAliasName, uint32 flags, size typeSize, uint32 alignment, const schemaMetadataDesc_t* pMetadata = NULL, uint32 numMetadata = 0 );

protected:
	// What a kind actually has to settle. Called by `Resolve`/`Unresolve`, which own
	// the state and the guards around it
	virtual void OnResolve();
	virtual void OnUnresolve();

	// Finds another described type by name and records that this one now points at
	// it. NULL when nothing is registered under the name
	ISchemaTypeInternal* ResolveDependency( const char* pTypeName );

private:
	// ISchemaTypeInternal interface
	virtual void AddDependent( ISchemaTypeInternal* pType ) override;
	virtual void RemoveDependent( ISchemaTypeInternal* pType ) override;

	virtual void SetScopeId( schemaScopeId_t scopeId ) override;

	void ClearDependencies();

	const char*							pName;
	const char*							pAliasName;
	size								typeSize;
	uint32								alignment;
	uint32								flags;
	schemaTypeKind_t					kind;
	schemaScopeId_t						scopeId;
	schemaTypeResolveState_t			resolveState;
	eastl::vector<ISchemaTypeInternal*> dependencies;  // The types THIS one points at
	eastl::vector<ISchemaTypeInternal*> dependents;	   // The types that point at this one
#if ENABLE_SCHEMA_METADATA
	CSchemaMetadata metadata;
#endif	// ENABLE_SCHEMA_METADATA
};

#include "schemasystem/schematypebase.inl"
