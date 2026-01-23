#pragma once
#include "studiorender/studioapi/istudioapi_texture.h"
#include "utils/stexdoc/stex_types.h"

//-----------------------------------------------------------------------------
// Helper for work with source STEX (Singularity Texture) files
//-----------------------------------------------------------------------------
class CSTEXSourceTextureDoc
{
public:
	CSTEXSourceTextureDoc();

	static void SetDefaultPixelFormat( studioAPIPixelFormat_t pixelFormat );

	// Load and save a texture document
	// NOTE: For use StdLib must be connected by ConnectStdLib
	bool LoadFromFile( const char* pPath );
	bool SaveFile( const char* pPath );
	void Clear();

	void AddSourcePath( const char* pSourcePath );
	void RemoveSourcePath( uint32 sourcePathIdx );

	void SetType( studioAPITextureType_t type );
	void SetGenerateMipMaps( bool bGenerateMipMaps );
	void SetAddressModeU( studioAPISamplerAddressMode_t addressModeU );
	void SetAddressModeV( studioAPISamplerAddressMode_t addressModeV );
	void SetAddressModeW( studioAPISamplerAddressMode_t addressModeW );
	void SetFilter( studioAPISamplerFilter_t filter );
	void SetPixelFormat( studioAPIPixelFormat_t pixelFormat );
	void SetMaxAnisotropy( uint32 maxAnisotropy );
	void SetOutputDir( const char* pOutputDir );

	bool								IsNeedGenerateMipMaps() const;
	studioAPISamplerAddressMode_t		GetAddressModeU() const;
	studioAPISamplerAddressMode_t		GetAddressModeV() const;
	studioAPISamplerAddressMode_t		GetAddressModeW() const;
	studioAPISamplerFilter_t			GetFilter() const;
	studioAPIPixelFormat_t				GetPixelFormat() const;
	studioAPITextureType_t				GetType() const;
	uint32								GetNumSourcePaths() const;
	const char*							GetSourcePath( uint32 sourcePathIdx );
	const eastl::vector<eastl::string>& GetSourcePaths() const;
	uint32								GetMaxAnisotropy() const;
	const char*							GetOutputDir() const;

private:
	// Is valid source paths number for a texture type
	bool IsValidNumSourcePaths( studioAPITextureType_t type, uint32 numSourcePaths, eastl::string& validateMsg ) const;

	bool						  bGenerateMipMaps;
	studioAPITextureType_t		  type;
	studioAPISamplerAddressMode_t addressModeU;
	studioAPISamplerAddressMode_t addressModeV;
	studioAPISamplerAddressMode_t addressModeW;
	studioAPISamplerFilter_t	  filter;
	studioAPIPixelFormat_t		  pixelFormat;
	uint32						  maxAnisotropy;
	eastl::string				  outputDir;
	eastl::vector<eastl::string>  sourcePaths;
	static studioAPIPixelFormat_t defaultPixelFormat;
};

#include "utils/stexdoc/stex_source_doc.inl"
