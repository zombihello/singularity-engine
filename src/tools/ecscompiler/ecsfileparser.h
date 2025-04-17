/**
 * @file
 * @addtogroup ecscompiler ecscompiler
 */

#ifndef ECSFILEPARSER_H
#define ECSFILEPARSER_H

#include "core/types.h"
#include "parserlib/file_context.h"
#include "tools/ecscompiler/ecsfieldstubs.h"

/**
 * @ingroup ecscompiler
 * @brief ECS file parser
 */
class CEcsFileParser
{
public:
	/**
	 * @brief Constructor
	 * @param stubs		Output ECS system stubs
	 */
	CEcsFileParser( CEcsSystemStub& stubs );

	/**
	 * @brief Parse a ECS file
	 * @param pPath			File path
	 * @param pCode			Source code from the file
	 * @return Return TRUE if the file has been successfully parsed, otherwise returns FALSE
	 */
	bool ParseFile( const achar* pPath, const achar* pCode );

	/**
	 * @brief Start module definition
	 * @param pContext		Module context
	 * @param pName			Module name
	 */
	void StartModule( const parserFileContext_t* pContext, const achar* pName );

	/**
	 * @brief Add using
	 * @param pContext		Using context
	 * @param pName			Using name
	 */
	void AddUsing( const parserFileContext_t* pContext, const achar* pName );

	/**
	 * @brief End definition of current module/component/system
	 * @param line				Number of line where is the end of definition
	 * @param pScopeStart		Context of the scope start
	 * @param pScopeEnd			Context of the scope end
	 */
	void EndDefinition( int32 line, const parserFileContext_t* pScopeStart, const parserFileContext_t* pScopeEnd );

	/**
	 * @brief Start component definition
	 * @param pContext	Component context
	 * @param pName		Component name
	 */
	void StartComponent( const parserFileContext_t* pContext, const achar* pName );

	/**
	 * @brief Add a field
	 * @param pContext			Field context
	 * @param pTypeContext		Field type context
	 * @param pName				Field name
	 * @param pType				Field type
	 */
	void AddField( const parserFileContext_t* pContext, const parserFileContext_t* pTypeContext, const achar* pName, const achar* pType );

	/**
	 * @brief Set a default field value
	 * @param pContext			Field context
	 * @param pValueContext		Value context
	 * @param pName				Field name
	 * @param pValue			Value
	 */
	void SetDefaultFieldValue( const parserFileContext_t* pContext, const parserFileContext_t* pValueContext, const achar* pName, const achar* pValue );

	/**
	 * @brief Emit parsing error
	 * @param pContext		Script file context
	 * @param pMessage		Message
	 */
	void EmitError( const parserFileContext_t* pContext, const achar* pMessage );

private:
	bool				bHasError;			/**< There were a parsing errors */
	CEcsSystemStub&		stubs;				/**< Output stubs */
	CEcsStubModule*		pCurrentModule;		/**< Current module */
	CEcsStubComponent*	pCurrentComponent;	/**< Current component */
};

#endif // !ECSFILEPARSER_H