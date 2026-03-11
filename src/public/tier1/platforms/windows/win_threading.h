#pragma once

// Windows version of a thread mutex
class CThreadMutexWindows
{
public:
	CThreadMutexWindows();
	~CThreadMutexWindows();

	void Lock();
	void Lock() const;
	void Unlock();
	void Unlock() const;

private:
	CRITICAL_SECTION criticalSection;
};

// Windows version of a thread event
class CThreadEventWindows
{
public:
	CThreadEventWindows( bool bManualReset = false, const char* pName = NULL );
	~CThreadEventWindows();

	void Trigger();
	void Reset();
	void Pulse();
	bool Wait( uint32 waitTime = -1 );

private:
	HANDLE handle;
};

// Windows version of a thread semaphore
class CThreadSemaphoreWindows
{
public:
	CThreadSemaphoreWindows( uint32 initialValue, uint32 maxValue, const char* pName = NULL );
	~CThreadSemaphoreWindows();

	bool Signal();
	bool Post( uint32 value );
	void Wait();
	bool Wait( uint32 milliseconds );
	bool TryWait();

private:
	HANDLE handle;
};

// Windows version of a thread
class CThreadWindows : public CThreadBase
{
public:
	CThreadWindows();
	~CThreadWindows();

	virtual bool Start( CRunnableObject* pRunnableObject, uint32 stackSize = 0 ) override;
	virtual void Stop( bool bShouldWait = false, int32 exitCode = 0 ) override;
	virtual void Suspend( bool bShouldPause = true ) override;
	virtual void WaitForCompletion() override;

private:
	static DWORD STDCALL ThreadMain( LPVOID pData );

	CThreadEventWindows* pThreadInitSyncEvent;
};

#include "tier1/platforms/windows/win_threading.inl"
