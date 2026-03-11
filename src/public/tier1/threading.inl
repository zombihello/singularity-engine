#pragma once

/*
==================
CThreadBase::SetName
==================
*/
FORCEINLINE void CThreadBase::SetName( const char* pName )
{
	S_Strncpy( CThreadBase::name, pName, sizeof( CThreadBase::name ) - 1 );
	CThreadBase::name[sizeof( CThreadBase::name ) - 1] = '\0';
	if ( IsAlive() )
	{
		Sys_SetThreadName( handle, pName );
	}
}

/*
==================
CThreadBase::SetPriority
==================
*/
FORCEINLINE void CThreadBase::SetPriority( threadPriority_t priority )
{
	CThreadBase::priority = priority;
	if ( IsAlive() )
	{
		Sys_SetThreadPriority( handle, priority );
	}
}

/*
==================
CThreadBase::IsAlive
==================
*/
FORCEINLINE bool CThreadBase::IsAlive() const
{
	return !!handle;
}

/*
==================
CThreadBase::GetName
==================
*/
FORCEINLINE const char* CThreadBase::GetName() const
{
	if ( !name[0] )
	{
		char* pPtrName = const_cast<char*>( name );
		S_Snprintf( pPtrName, sizeof( name ) - 1, "Thread(%p/%p)", this, handle );
		pPtrName[sizeof( name ) - 1] = '\0';
	}
	return name;
}

/*
==================
CThreadBase::GetPriority
==================
*/
FORCEINLINE threadPriority_t CThreadBase::GetPriority() const
{
	return priority;
}

/*
==================
CThreadBase::GetHandle
==================
*/
FORCEINLINE threadHandle_t CThreadBase::GetHandle() const
{
	return handle;
}

/*
==================
CThreadBase::GetId
==================
*/
FORCEINLINE threadId_t CThreadBase::GetId() const
{
	return id;
}

/*
==================
CThreadBase::GetExitCode
==================
*/
FORCEINLINE int32 CThreadBase::GetExitCode() const
{
	return exitCode;
}

/*
==================
CScopeLock::CScopeLock
==================
*/
FORCEINLINE CScopeLock::CScopeLock( CThreadMutex& mutex )
	: syncObject( mutex )
{
	syncObject.Lock();
}

/*
==================
CScopeLock::~CScopeLock
==================
*/
FORCEINLINE CScopeLock::~CScopeLock()
{
	syncObject.Unlock();
}
