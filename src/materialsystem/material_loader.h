#pragma once
#include "resourcesystem/iresourcetype.h"

//-----------------------------------------------------------------------------
// Material loader
//-----------------------------------------------------------------------------
class CMaterialLoader : public IResourceTypeLoader
{
public:
	// IResourceTypeLoader interface
	virtual bool		Load( const char* pPath, void* pData ) const override;
	virtual const char* GetFormatName() const override;
};
