#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier0/iprofiler.h"
#include "tier1/filetools.h"
#include "tier1/keyvalues.h"

//-----------------------------------------------------------------------------
// Helper for work with toolsinfo.txt files
//-----------------------------------------------------------------------------
struct toolsInfoSearchPath_t
{
	eastl::string id;
	eastl::string path;
};

class CToolsInfoDoc
{
public:
	CToolsInfoDoc();

	// Methods for load from file/buffer and clear the document
	// NOTE: For LoadFromFile must be connected Tier1
	bool LoadFromFile( const char* pPath );
	bool LoadFromBuffer( const char* pBuffer );
	bool LoadFromStream( IStreamDataReader* pStreamReader );
	void Clear();

	const eastl::vector<toolsInfoSearchPath_t>& GetSearchPaths() const;
	const eastl::vector<eastl::string>&			GetResourceCompilers() const;
	bool										IsLoaded() const;

private:
	bool								 bLoaded;
	eastl::vector<toolsInfoSearchPath_t> searchPaths;
	eastl::vector<eastl::string>		 resourceCompilers;
};

#include "utils/toolsinfo/toolsinfo.inl"
