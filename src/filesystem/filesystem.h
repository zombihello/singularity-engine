#pragma once
#include <string>
#include <vector>

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
	virtual TRefPtr<IStreamDataReader> CreateFileReader( const achar* pPath, uint32 flags = FILE_READ_NONE ) override;

	// This is method looks file in the search paths and at the first one create a new file
	virtual TRefPtr<IStreamDataWriter> CreateFileWriter( const achar* pPath, uint32 flags = FILE_WRITE_NONE ) override;

	// This is method looks files in the search paths or in the first if it set
	virtual TRefPtr<IPathArrayResult> FindFiles( const achar* pPath, bool bFiles, bool bDirectories, bool bLookAllPathIDs = true ) override;

	// This is method looks a DLL module in the search paths and first opened returns
	virtual dllHandle_t LoadModule( const achar* pDLLName ) override;
	virtual void UnloadModule( dllHandle_t dllHandle ) override;

	// This method looks for a file in the search paths and the first one deletes or all if this flag is set
	virtual bool DeleteFile( const achar* pPath, bool bDeleteAllPathIDs = false, bool bEvenReadOnly = false ) override;

	// This is method looks a path in the search paths and at the first one make a new directory
	virtual bool MakeDirectory( const achar* pPath ) override;

	// This method looks for a directory in the search paths and the first one deletes or all if this flag is set.
	virtual bool DeleteDirectory( const achar* pPath, bool bDeleteAllPathIDs = false, bool bEvenReadOnly = false ) override;

	// This method looks for a file in the search paths and the first one copy into 
	// the first found search path in pDestPath
	virtual copyMoveResult_t CopyFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

	// This method looks for directories in the search paths and copy they into
	// the first found search path in pDestPath
	virtual copyMoveResult_t CopyDirectory( const achar* pSrcPath, const achar* pDestPath, bool bCopyAllPathIDs = false, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

	// This method looks for a file in the search paths and the first one move into
	// the first found search path in pDestPath
	virtual copyMoveResult_t MoveFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

	// This method looks for directories in the search paths and move they into
	// the first found search path in pDestPath
	virtual copyMoveResult_t MoveDirectory( const achar* pSrcPath, const achar* pDestPath, bool bMoveAllPathIDs = false, bool bReplaceExisting = false, bool bEvenReadOnly = false ) override;

	// This is method looks for a file or a directory in the search paths and when first was found returns TRUE
	virtual bool IsFileExists( const achar* pPath ) const override;

	// This is method looks for a file or a directory in the search paths and when first was found returns TRUE if it is directory
	virtual bool IsFileDirectory( const achar* pPath ) const override;

	// Functions to work with search paths
	virtual void AddSearchPath( const achar* pSearchPath, const achar* pPathID ) override;
	virtual void RemoveAllSearchPaths() override;
	virtual void RemoveSearchPath( const achar* pPathID ) override;
	virtual TRefPtr<IPathArrayResult> GetSearchPath( const achar* pPathID ) const override;
	virtual uint32 GetNumSearchPaths() const override;

private:
    // NOTE: Internal functions not support path IDs
    bool MakeDirectoryInternal( const achar* pPath );
	bool DeleteDirectoryInternal( const achar* pPath, bool bEvenReadOnly = false );
    copyMoveResult_t CopyDirectoryInternal( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );
    copyMoveResult_t MoveDirectoryInternal( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false );

	// This method looks for UNC-type filename specifier, which should be used instead of passing in path ID.
	// So if it finds //GAME/cfg/config.cfg, set pointer pFilePath to the begin of file path, set pointer pPathID 
	// to the begin of path ID and set length of pPathID into lengthPathID
    void ParsePathID( const achar* pPath, const achar*& pFilePath, const achar*& pPathID, uint32& lengthPathID ) const;
    void ComputeFullPath( const achar* pFilePath, const CSearchPath* pSearchPath, std::string& destPath ) const;

    std::vector<CSearchPath>      searchPaths;
};