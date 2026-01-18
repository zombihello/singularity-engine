#pragma once
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// IConVars/IConCmds are marked as having a particular DLL identifier
//-----------------------------------------------------------------------------
typedef uint32 cvarDLLIdentifier_t;

//-----------------------------------------------------------------------------
// IConVar and IConCmd flags
//-----------------------------------------------------------------------------
enum cvarFlags_t
{
	FCVAR_NONE	  = 0,
	FCVAR_CHEAT	  = BIT( 0 ),
	FCVAR_ARCHIVE = BIT( 1 )
};

//-----------------------------------------------------------------------------
// The base console invoked command/variable interface
//-----------------------------------------------------------------------------
class IConCmdBase
{
public:
	virtual bool   IsFlagSet( uint32 flag ) const = 0;
	virtual uint32 GetFlags() const				  = 0;
	virtual void   AddFlags( uint32 flags )		  = 0;

	virtual bool				IsCommand() const					  = 0;
	virtual void				SetHelpText( const char* pHelpText ) = 0;
	virtual const char*		GetName() const						  = 0;
	virtual const char*		GetHelpText() const					  = 0;
	virtual cvarDLLIdentifier_t GetDLLIdentifier() const			  = 0;

	// Set/get next cvar in the global list
	virtual void		 SetNext( IConCmdBase* pNext ) = 0;
	virtual IConCmdBase* GetNext() const			   = 0;

	// Internal usage only!
	virtual void SetRegistered( bool bRegistered ) = 0;
	virtual bool IsRegistered() const			   = 0;
};

//-----------------------------------------------------------------------------
// The console invoked command interface
//-----------------------------------------------------------------------------
class IConCmd : public IConCmdBase
{
public:
	virtual void Exec( uint32 argc, const char** argv ) = 0;
};
