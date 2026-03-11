#include "pch_tier1.h"
#include "tier1/threading.h"

/*
==================
CRunnableObject::Init
==================
*/
bool CRunnableObject::Init()
{
	return true;
}

/*
==================
CRunnableObject::Stop
==================
*/
void CRunnableObject::Stop()
{
}

/*
==================
CRunnableObject::Exit
==================
*/
void CRunnableObject::Exit()
{
}

/*
==================
CThreadBase::CThreadBase
==================
*/
CThreadBase::CThreadBase()
	: exitCode( -1 )
	, id( INVALID_THREAD_ID )
	, handle( INVALID_THREAD_HANDLE )
	, pRunnableObject( NULL )
	, priority( THREAD_PRIOR_NORMAL )
{
	name[0] = '\0';
}
