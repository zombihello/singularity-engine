#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "filesystem/ifilesystem.h"
#include "filesystem/searchpath.h"

//-----------------------------------------------------------------------------
// File system
//-----------------------------------------------------------------------------
class CFileSystem : public CBaseAppSystem<IFileSystem>
{
public:
	friend CSearchPathIterator;

	CFileSystem();
	virtual ~CFileSystem();

	// IAppSystem interface
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// IFileSystem interface
	// This is method looks file in the search paths and first opened returns
	virtual CRefPtr<IStreamDataReader> CreateFileReader( const char* pPath, uint32 flags = FILE_READ_NONE ) override;

	// This is method looks file in the search paths and at the first one create a new file
	virtual CRefPtr<IStreamDataWriter> CreateFileWriter( const char* pPath, uint32 flags = FILE_WRITE_NONE ) override;

	// This is method looks files in the search paths or in the first if it set
	virtual CRefPtr<IPathArrayResult> FindFiles( const char* pPath, const char* pPattern, bool bFiles, bool bDirectories, bool bLookAllPathIDs = true ) override;

	// This is method looks a DLL module in the search paths and first opened returns
	virtual dllHandle_t LoadModule( const char* pDLLName ) override;
	virtual void		UnloadModule( dllHandle_t dllHandle ) override;

	// This method looks for a file in the search paths and the first one deletes or all if this flag is set
	virtual bool DeleteFile( const char* pPath, bool bDeleteAllPathIDs = false, bool bEvenReadOnly = false ) override;

	// This is method looks a path in the search paths and at the first one make a new directory
	virtual bool MakeDirectory( const char* pPath ) override;

	// This method looks for a directory in the search paths and the first one deletes or all if this flag is set.
	virtual bool DeleteDirectory( const char* pPath, bool bDeleteAllPathIDs = false, bool bEvenReadOnly = false ) override;

	// This method looks for a file in the search paths and the first one copy into
	// the first found search path in pDestPath
	virtual copyMoveResult_t CopyFile( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

	// This method looks for directories in the search paths and copy they into
	// the first found search path in pDestPath
	virtual copyMoveResult_t CopyDirectory( const char* pSrcPath, const char* pDestPath, bool bCopyAllPathIDs = false, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

	// This method looks for a file in the search paths and the first one move into
	// the first found search path in pDestPath
	virtual copyMoveResult_t MoveFile( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

	// This method looks for directories in the search paths and move they into
	// the first found search path in pDestPath
	virtual copyMoveResult_t MoveDirectory( const char* pSrcPath, const char* pDestPath, bool bMoveAllPathIDs = false, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

	// This method looks for directories in the search paths and the first one select as the current directory
	virtual bool SetCurrentDirectory( const char* pPath ) override;

	// This is method looks for a file or a directory in the search paths and when first was found returns TRUE
	virtual bool IsFileExists( const char* pPath ) const override;

	// This is method looks for a file or a directory in the search paths and when first was found returns TRUE if it is directory
	virtual bool IsFileDirectory( const char* pPath ) const override;

	// Functions to get path to some directories
	virtual const char* GetUserDirectory() const override;
	virtual bool		GetCurrentDirectory( char* pDest, uint32 maxLen ) const override;

	// Functions to work with search paths
	virtual void					  AddSearchPath( const char* pSearchPath, const char* pPathID ) override;
	virtual void					  RemoveAllSearchPaths() override;
	virtual void					  RemoveSearchPath( const char* pPathID ) override;
	virtual CRefPtr<IPathArrayResult> GetSearchPath( const char* pPathID ) const override;
	virtual uint32					  GetNumSearchPaths() const override;

private:
	// NOTE: Internal functions not support path IDs
	bool			 MakeDirectoryInternal( const char* pPath );
	bool			 DeleteDirectoryInternal( const char* pPath, bool bEvenReadOnly = false );
	copyMoveResult_t CopyDirectoryInternal( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );
	copyMoveResult_t MoveDirectoryInternal( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );

	// This method looks for UNC-type filename specifier, which should be used instead of passing in path ID.
	// So if it finds //GAME/cfg/config.cfg, set pointer pFilePath to the begin of file path, set pointer pPathID
	// to the begin of path ID and set length of pPathID into lengthPathID
	void ParsePathID( const char* pPath, const char*& pFilePath, const char*& pPathID, uint32& lengthPathID ) const;
	void ComputeFullPath( const char* pFilePath, const CSearchPath* pSearchPath, eastl::string& destPath ) const;

	eastl::vector<CSearchPath> searchPaths;
};
