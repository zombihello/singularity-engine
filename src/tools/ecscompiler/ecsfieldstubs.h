/**
 * @file
 * @addtogroup ecscompiler ecscompiler
 */

#ifndef ECSFIELDSTUBS_H
#define ECSFIELDSTUBS_H

#include <vector>

#include "stdlib/refcount.h"
#include "parserlib/file_context.h"

/**
 * @ingroup ecscompiler
 * @brief ECS scope of a stub
 */
struct ecsScopeStub_t
{
	parserFileContext_t		startContext;	/**< Context of the start context */
	parserFileContext_t		endContext;		/**< Context of the end context */
};

/**
 * @ingroup ecscompiler
 * @brief ECS base stub
 */
class CEcsStubBase : public TRefCounted<IRefCounted>
{
public:
	/**
	 * @brief Constructor
	 * @param context	Stub context
	 * @param pName		Stub name
	 */
	CEcsStubBase( const parserFileContext_t& context, const achar* pName );

	/**
	 * @brief Get the stub context
	 * @return Return the stub context
	 */
	FORCEINLINE const parserFileContext_t& GetContext() const
	{
		return context;
	}

	/**
	 * @brief Get the stub name
	 * @return Return the sub name. If isn't return empty string
	 */
	FORCEINLINE const achar* GetName() const
	{
		return name.c_str();
	}

protected:
	parserFileContext_t		context;	/**< Context */
	std::string				name;		/**< Name */
};

/**
 * @ingroup ecscompiler
 * @brief ECS default field value stub
 */
class CEcsStubDefaultFieldValue : public CEcsStubBase
{
public:
	/**
	 * @brief Constructor
	 * @param context		Stub context
	 * @param valueContext	Value context
	 * @param pName			Stub name
	 * @param pValue		Value
	 */
	CEcsStubDefaultFieldValue( const parserFileContext_t& context, const parserFileContext_t& valueContext, const achar* pName, const achar* pValue );

	/**
	 * @brief Get the value context
	 * @return Return the value context
	 */
	FORCEINLINE const parserFileContext_t& GetValueContext() const
	{
		return valueContext;
	}

	/**
	 * @brief Get the value
	 * @return Return the value. If isn't return empty string
	 */
	FORCEINLINE const achar* GetValue() const
	{
		return value.c_str();
	}

private:
	parserFileContext_t		valueContext;	/**< Value context */
	std::string				value;			/**< Value */
};

/**
 * @ingroup ecscompiler
 * @brief ECS field stub
 */
class CEcsStubField : public CEcsStubBase
{
public:
	/**
	 * @brief Constructor
	 * @param context		Stub context
	 * @param typeContext	Type context
	 * @param pName			Stub name
	 * @param pType			Type
	 */
	CEcsStubField( const parserFileContext_t& context, const parserFileContext_t& typeContext, const achar* pName, const achar* pType );

	/**
	 * @brief Get the type context
	 * @return Return the type context
	 */
	FORCEINLINE const parserFileContext_t& GetTypeContext() const
	{
		return typeContext;
	}

	/**
	 * @brief Get the type
	 * @return Return the type. If isn't return empty string
	 */
	FORCEINLINE const achar* GetType() const
	{
		return type.c_str();
	}

private:
	parserFileContext_t		typeContext;	/**< Type context */
	std::string				type;			/**< Type */
};

/**
 * @ingroup ecscompiler
 * @brief ECS component stub
 */
class CEcsStubComponent : public CEcsStubBase
{
public:
	/**
	 * @brief Constructor
	 * @param context	Stub context
	 * @param pName		Stub name
	 */
	CEcsStubComponent( const parserFileContext_t& context, const achar* pName );

	/**
	 * @brief Add a default field value
	 * @param pDefaultFieldValue	Default field value
	 */
	FORCEINLINE void AddDefaultFieldValue( CEcsStubDefaultFieldValue* pDefaultFieldValue )
	{
		defaultFieldValues.emplace_back( pDefaultFieldValue );
	}

	/**
	 * @brief Add a field
	 * @param pField	Field
	 */
	FORCEINLINE void AddField( CEcsStubField* pField )
	{
		fields.emplace_back( pField );
	}

	/**
	 * @brief Get default field values
	 * @return Return default field values
	 */
	FORCEINLINE const std::vector<TRefPtr<CEcsStubDefaultFieldValue>>& GetDefaultFieldValues() const
	{
		return defaultFieldValues;
	}

	/**
	 * @brief Get fields
	 * @return Return fields
	 */
	FORCEINLINE const std::vector<TRefPtr<CEcsStubField>>& GetFields() const
	{
		return fields;
	}

	/**
	 * @brief Get the component scope
	 * @return Return reference to the component scope
	 */
	FORCEINLINE ecsScopeStub_t& GetScope()
	{
		return scope;
	}

private:
	ecsScopeStub_t										scope;					/**< Scope of the component */
	std::vector<TRefPtr<CEcsStubDefaultFieldValue>>		defaultFieldValues;		/**< Default field values */
	std::vector<TRefPtr<CEcsStubField>>					fields;					/**< Fields */
};

/**
 * @ingroup ecscompiler
 * @brief ECS using stub
 */
class CEcsStubUsing : public CEcsStubBase
{
public:
	/**
	 * @brief Constructor
	 * @param context	Stub context
	 * @param pName		Stub name
	 */
	CEcsStubUsing( const parserFileContext_t& context, const achar* pName );
};

/**
 * @ingroup ecscompiler
 * @brief ECS module stub
 */
class CEcsStubModule : public CEcsStubBase
{
public:
	/**
	 * @brief Constructor
	 * @param context	Stub context
	 * @param pName		Stub name
	 */
	CEcsStubModule( const parserFileContext_t& context, const achar* pName );

	/**
	 * @brief Add a using
	 * @param pUsing	Using
	 */
	FORCEINLINE void AddUsing( CEcsStubUsing* pUsing )
	{
		usings.emplace_back( pUsing );
	}

	/**
	 * @brief Add a component
	 * @param pComponent	Component
	 */
	FORCEINLINE void AddComponent( CEcsStubComponent* pComponent )
	{
		components.emplace_back( pComponent );
	}

	/**
	 * @brief Get usings
	 * @return Return usings
	 */
	FORCEINLINE const std::vector<TRefPtr<CEcsStubUsing>>& GetUsings() const
	{
		return usings;
	}

	/**
	 * @brief Get components
	 * @return Return components
	 */
	FORCEINLINE const std::vector<TRefPtr<CEcsStubComponent>>& GetComponents() const
	{
		return components;
	}

	/**
	 * @brief Get the module scope
	 * @return Return reference to the module scope
	 */
	FORCEINLINE ecsScopeStub_t& GetScope()
	{
		return scope;
	}

private:
	ecsScopeStub_t								scope;			/**< Scope of the module */
	std::vector<TRefPtr<CEcsStubUsing>>			usings;			/**< Usings */
	std::vector<TRefPtr<CEcsStubComponent>>		components;		/**< Components */
};

/**
 * @ingroup ecscompiler
 * @brief ECS system stub
 */
class CEcsSystemStub
{
public:
	/**
	 * @brief Add a module
	 * @param pModule	Module
	 */
	FORCEINLINE void AddModule( CEcsStubModule* pModule )
	{
		modules.emplace_back( pModule );
	}

	/**
	 * @brief Get modules
	 * @return Return modules
	 */
	FORCEINLINE const std::vector<TRefPtr<CEcsStubModule>>& GetModules() const
	{
		return modules;
	}

private:
	std::vector<TRefPtr<CEcsStubModule>>		modules;	/**< Modules */
};

#endif // !ECSFIELDSTUBS_H