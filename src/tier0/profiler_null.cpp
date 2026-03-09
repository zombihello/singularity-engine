#include "pch_tier0.h"

#if !ENABLE_PROFILING || PLATFORM_USE_NULL_PROFILER
	#include "tier0/iprofiler.h"

//-----------------------------------------------------------------------------
// Null profiler
//-----------------------------------------------------------------------------
class CProfilerNull : public IProfiler
{
public:
	// IProfiler interface
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void Update() override;

	virtual void RegisterScope( profilerScopeData_t& scopeData ) override;
	virtual void BeginScope( const profilerScopeData_t& scopeData, const char* pScopeName = NULL ) override;
	virtual void EndScope( const profilerScopeData_t& scopeData ) override;

	virtual void BeginThreadScope( const char* pThreadName ) override;
	virtual void EndThreadScope() override;

	virtual void BeginMarkScope( const char* pMarkName ) override;
	virtual void EndMarkScope( const char* pMarkName ) override;
	virtual void NextFrame() override;

	virtual void SendMessage( const char* pMessage ) override;
	virtual void SendMemAlloc( const void* pPtr, size numBytes, const char* pAllocatorName = NULL ) override;
	virtual void SendMemFree( const void* pPtr, const char* pAllocatorName = NULL ) override;

	virtual bool IsInited() const override;
	virtual bool IsConnected() const override;
};

/*
==================
CProfilerNull::Init
==================
*/
void CProfilerNull::Init()
{
}

/*
==================
CProfilerNull::Shutdown
==================
*/
void CProfilerNull::Shutdown()
{
}

/*
==================
CProfilerNull::Update
==================
*/
void CProfilerNull::Update()
{
}

/*
==================
CProfilerNull::RegisterScope
==================
*/
void CProfilerNull::RegisterScope( profilerScopeData_t& scopeData )
{
}

/*
==================
CProfilerNull::BeginScope
==================
*/
void CProfilerNull::BeginScope( const profilerScopeData_t& scopeData, const char* pScopeName /* = NULL */ )
{
}

/*
==================
CProfilerNull::EndScope
==================
*/
void CProfilerNull::EndScope( const profilerScopeData_t& scopeData )
{
}

/*
==================
CProfilerNull::BeginThreadScope
==================
*/
void CProfilerNull::BeginThreadScope( const char* pThreadName )
{
}

/*
==================
CProfilerNull::EndThreadScope
==================
*/
void CProfilerNull::EndThreadScope()
{
}

/*
==================
CProfilerNull::BeginMarkScope
==================
*/
void CProfilerNull::BeginMarkScope( const char* pMarkName )
{
}

/*
==================
CProfilerNull::EndMarkScope
==================
*/
void CProfilerNull::EndMarkScope( const char* pMarkName )
{
}

/*
==================
CProfilerNull::NextFrame
==================
*/
void CProfilerNull::NextFrame()
{
}

/*
==================
CProfilerNull::SendMessage
==================
*/
void CProfilerNull::SendMessage( const char* pMessage )
{
}

/*
==================
CProfilerNull::SendMemAlloc
==================
*/
void CProfilerNull::SendMemAlloc( const void* pPtr, size numBytes, const char* pAllocatorName /* = NULL */ )
{
}

/*
==================
CProfilerNull::SendMemFree
==================
*/
void CProfilerNull::SendMemFree( const void* pPtr, const char* pAllocatorName /* = NULL */ )
{
}

/*
==================
CProfilerNull::IsInited
==================
*/
bool CProfilerNull::IsInited() const
{
	return false;
}

/*
==================
CProfilerNull::IsConnected
==================
*/
bool CProfilerNull::IsConnected() const
{
	return false;
}

/*
==================
Profiler
==================
*/
IProfiler* Profiler()
{
	static CProfilerNull s_profilerNull;
	return &s_profilerNull;
}
#endif	// !ENABLE_PROFILING || PLATFORM_USE_NULL_PROFILER
