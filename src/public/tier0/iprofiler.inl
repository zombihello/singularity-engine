#pragma once

/*
==================
profilerScopeData_t::profilerScopeData_t
==================
*/
FORCEINLINE profilerScopeData_t::profilerScopeData_t( const char* pScopeName, const char* pFunctionName, const char* pFileName, uint32 line, profilerScopeGroup_t scopeGroup /* = PROFILER_SCOPE_GROUP_NONE */ )
	: pScopeName( pScopeName )
	, pFunctionName( pFunctionName )
	, pFileName( pFileName )
	, line( line )
	, id( INVALID_INDEX )
	, scopeGroup( scopeGroup )
{
}

/*
==================
CProfilerScope::CProfilerScope
==================
*/
FORCEINLINE CProfilerScope::CProfilerScope( const profilerScopeData_t& scopeData, const char* pScopeName /* = NULL */ )
	: scopeData( scopeData )
	, bActive( Profiler()->IsConnected() )
{
	if ( bActive )
	{
		Profiler()->BeginScope( scopeData, pScopeName );
	}
}

/*
==================
CProfilerScope::~CProfilerScope
==================
*/
FORCEINLINE CProfilerScope::~CProfilerScope()
{
	if ( bActive )
	{
		Profiler()->EndScope( scopeData );
	}
}

/*
==================
CProfilerThreadScope::CProfilerThreadScope
==================
*/
FORCEINLINE CProfilerThreadScope::CProfilerThreadScope( const char* pThreadName )
{
	Profiler()->BeginThreadScope( pThreadName );
}

/*
==================
CProfilerThreadScope::~CProfilerThreadScope
==================
*/
FORCEINLINE CProfilerThreadScope::~CProfilerThreadScope()
{
	Profiler()->EndThreadScope();
}

/*
==================
CProfilerMarkScope::CProfilerMarkScope
==================
*/
FORCEINLINE CProfilerMarkScope::CProfilerMarkScope( const char* pMarkName )
	: pMarkName( pMarkName )
	, bActive( Profiler()->IsConnected() )
{
	if ( bActive )
	{
		Profiler()->BeginMarkScope( pMarkName );
	}
}

/*
==================
CProfilerMarkScope::~CProfilerMarkScope
==================
*/
FORCEINLINE CProfilerMarkScope::~CProfilerMarkScope()
{
	if ( bActive )
	{
		Profiler()->EndMarkScope( pMarkName );
	}
}
