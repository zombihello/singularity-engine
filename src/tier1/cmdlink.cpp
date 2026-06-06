#include "pch_tier1.h"
#include "tier1/cmdlink.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
cmdLink_t*		   CCmdsLocalRegister::s_pCmdList	   = NULL;
cmdDLLIdentifier_t CCmdsLocalRegister::s_dllIdentifier = INVALID_INDEX;

/*
==================
LinkCmds
==================
*/
void LinkCmds( uint32 flags /* = CMD_FLAG_NONE */ )
{
	if ( !g_pCmdSystem || CCmdsLocalRegister::s_dllIdentifier != INVALID_INDEX )
	{
		return;
	}

	CCmdsLocalRegister::s_dllIdentifier = g_pCmdSystem->AllocateDLLIdentifier();
	for ( cmdLink_t* pCurCmd = CCmdsLocalRegister::s_pCmdList; pCurCmd; pCurCmd = pCurCmd->pNext )
	{
		g_pCmdSystem->AddCommand( pCurCmd->pName, pCurCmd->pFunctionFn, pCurCmd->flags | flags, pCurCmd->pDescription, CCmdsLocalRegister::s_dllIdentifier );
	}
}

/*
==================
UnlinkCmds
==================
*/
void UnlinkCmds()
{
	if ( !g_pCmdSystem || CCmdsLocalRegister::s_dllIdentifier == INVALID_INDEX )
	{
		return;
	}

	g_pCmdSystem->RemoveAllCommands( CCmdsLocalRegister::s_dllIdentifier );
	CCmdsLocalRegister::s_dllIdentifier = INVALID_INDEX;
}
