#pragma once
#include "stdlib/types.h"
#include "appframework/iappsystem.h"
#include "stdlib/refcount.h"
#include "stdlib/istreamdata.h"

//-----------------------------------------------------------------------------
// File modes and result types
//-----------------------------------------------------------------------------
enum fileRead_t
{
	FILE_READ_NONE	  = 0,
	FILE_READ_NO_FAIL = 1 << 1	// The file must open, otherwise there will be a fatal error
};

enum fileWrite_t
{
	FILE_WRITE_NONE	   = 0,
	FILE_WRITE_NO_FAIL = 1 << 1,  // The file must open, otherwise there will be a fatal error
	FILE_WRITE_APPEND  = 1 << 2	  // Don't clear file before operations
};

enum copyMoveResult_t
{
	COPYMOVE_RESULT_OK,
	COPYMOVE_RESULT_MISC_FAIL,
	COPYMOVE_RESULT_READ_FAIL,
	COPYMOVE_RESULT_WRITE_FAIL,
	COPYMOVE_RESULT_CANCELED
};

//-----------------------------------------------------------------------------
// Path array result interface
//-----------------------------------------------------------------------------
class IPathArrayResult : public IRefCounted
{
public:
	virtual const char* GetPath( uint32 index ) const = 0;
	virtual uint32		GetNum() const				  = 0;
	virtual bool		IsEmpty() const				  = 0;
};

//-----------------------------------------------------------------------------
// File system interface
// NOTE: You can set path ID use at the begin '//',
// for example: //GAME/cfg/config.cfg, //GAME/bin/game.dll
//-----------------------------------------------------------------------------
#define FILESYSTEM_INTERFACE_VERSION "SFileSystem001"
class IFileSystem : public IAppSystem
{
public:
	// This is method looks file in the search paths and first opened returns
	virtual TRefPtr<IStreamDataReader> CreateFileReader( const char* pPath, uint32 flags = FILE_READ_NONE ) = 0;

	// This is method looks file in the search paths and at the first one create a new file
	virtual TRefPtr<IStreamDataWriter> CreateFileWriter( const char* pPath, uint32 flags = FILE_WRITE_NONE ) = 0;

	// This is method looks files in the search paths or in the first if it set
	virtual TRefPtr<IPathArrayResult> FindFiles( const char* pPath, bool bFiles, bool bDirectories, bool bLookAllPathIDs = true ) = 0;

	// This is method looks a DLL module in the search paths and first opened returns
	virtual dllHandle_t LoadModule( const char* pDLLName )	  = 0;
	virtual void		UnloadModule( dllHandle_t dllHandle ) = 0;

	// This method looks for a file in the search paths and the first one deletes or all if this flag is set
	virtual bool DeleteFile( const char* pPath, bool bDeleteAllPathIDs = false, bool bEvenReadOnly = false ) = 0;

	// This is method looks a path in the search paths and at the first one make a new directory
	virtual bool MakeDirectory( const char* pPath ) = 0;

	// This method looks for a directory in the search paths and the first one deletes or all if this flag is set.
	virtual bool DeleteDirectory( const char* pPath, bool bDeleteAllPathIDs = false, bool bEvenReadOnly = false ) = 0;

	// This method looks for a file in the search paths and the first one copy into
	// the first found search path in pDestPath
	virtual copyMoveResult_t CopyFile( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false ) = 0;

	// This method looks for directories in the search paths and copy they into
	// the first found search path in pDestPath
	virtual copyMoveResult_t CopyDirectory( const char* pSrcPath, const char* pDestPath, bool bCopyAllPathIDs = false, bool bReplaceExisting = false, bool bEvenReadOnly = false ) = 0;

	// This method looks for a file in the search paths and the first one move into
	// the first found search path in pDestPath
	virtual copyMoveResult_t MoveFile( const char* pSrcPath, const char* pDestPath, bool bReplaceExisting = false, bool bEvenReadOnly = false ) = 0;

	// This method looks for directories in the search paths and move they into
	// the first found search path in pDestPath
	virtual copyMoveResult_t MoveDirectory( const char* pSrcPath, const char* pDestPath, bool bMoveAllPathIDs = false, bool bReplaceExisting = false, bool bEvenReadOnly = false ) = 0;

	// This is method looks for a file or a directory in the search paths and when first was found returns TRUE
	virtual bool IsFileExists( const char* pPath ) const = 0;

	// This is method looks for a file or a directory in the search paths and when first was found returns TRUE if it is directory
	virtual bool IsFileDirectory( const char* pPath ) const = 0;

	// Functions to work with search paths
	virtual void					  AddSearchPath( const char* pSearchPath, const char* pPathID ) = 0;
	virtual void					  RemoveAllSearchPaths()										= 0;
	virtual void					  RemoveSearchPath( const char* pPathID )						= 0;
	virtual TRefPtr<IPathArrayResult> GetSearchPath( const char* pPathID ) const					= 0;
	virtual uint32					  GetNumSearchPaths() const										= 0;
};
