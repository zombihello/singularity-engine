#pragma once
#include "stdlib/math/math.h"
#include "appframework/iappsystem.h"

//-----------------------------------------------------------------------------
// Map tool interface
//-----------------------------------------------------------------------------
struct resourceToolMapEntity_t
{
	const achar*	pEntityDesc;
	const achar*	pName;
};


struct resourceToolCompileMapParams_t
{
	const achar*					pDestPath;
	const resourceToolMapEntity_t*	pEntities;
	uint32							numEntities;
};


#define MAP_TOOL_INTERFACE_VERSION		"SMapTool001"
class IMapTool : public IAppSystem
{
public:
	virtual bool CompileMap( const resourceToolCompileMapParams_t& compileParams ) const = 0;
};
