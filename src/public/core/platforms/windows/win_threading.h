#pragma once

//-----------------------------------------------------------------------------
// Windows version of a thread mutex
//-----------------------------------------------------------------------------
class CORE_CLASS CWindowsThreadMutex : public IThreadMutex
{
public:
	CWindowsThreadMutex();
	virtual ~CWindowsThreadMutex();

	virtual void Lock() override;
	virtual void Lock() const override;
	virtual void Unlock() override;
	virtual void Unlock() const override;

private:
	CRITICAL_SECTION criticalSection;
};

//-----------------------------------------------------------------------------
// Windows version of a thread event
//-----------------------------------------------------------------------------
class CORE_CLASS CWindowsThreadEvent : public IThreadEvent
{
public:
	CWindowsThreadEvent( bool bManualReset = false, const char* pName = NULL );
	virtual ~CWindowsThreadEvent();

	virtual void Trigger() override;
	virtual void Reset() override;
	virtual void Pulse() override;
	virtual bool Wait( uint32 waitTime = -1 ) override;

private:
	HANDLE handle;
};

//-----------------------------------------------------------------------------
// Windows version of a thread semaphore
//-----------------------------------------------------------------------------
class CORE_CLASS CWindowsThreadSemaphore : public IThreadSemaphore
{
public:
	CWindowsThreadSemaphore( uint32 initialValue, uint32 maxValue, const char* pName = NULL );
	virtual ~CWindowsThreadSemaphore();

	virtual bool Signal() override;
	virtual bool Post( uint32 value ) override;
	virtual void Wait() override;
	virtual bool Wait( uint32 milliseconds ) override;
	virtual bool TryWait() override;

private:
	HANDLE handle;
};

//-----------------------------------------------------------------------------
// Windows version of a base thread class
//-----------------------------------------------------------------------------
class CORE_CLASS CWindowsThread : public IThread
{
public:
	CWindowsThread();
	virtual ~CWindowsThread();

	virtual bool Start( uint32 stackSize = 0 ) override;
	virtual void Stop( bool bShouldWait = false, int32 exitCode = 0 ) override;
	virtual void Suspend( bool bShouldPause = true ) override;
	virtual void WaitForCompletion() override;

	virtual void SetName( const char* pName ) override;
	virtual void SetPriority( threadPriority_t priority ) override;

	virtual bool			 IsAlive() const override;
	virtual const char*	 GetName() const override;
	virtual threadPriority_t GetPriority() const override;
	virtual threadHandle_t	 GetThreadHandle() const override;
	virtual threadId_t		 GetThreadId() const override;
	virtual int32			 GetExitCode() const override;

protected:
	virtual bool ThreadInit() override;
	virtual void ThreadStop() override;
	virtual void ThreadExit() override;

private:
	static DWORD STDCALL ThreadMain( LPVOID pThis );

	int32				 exitCode;
	threadPriority_t	 threadPriority;
	char				 name[64];
	threadHandle_t		 handle;
	threadId_t			 id;
	CWindowsThreadEvent* pThreadInitSyncEvent;
};
