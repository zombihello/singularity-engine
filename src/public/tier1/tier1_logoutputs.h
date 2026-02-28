#pragma once
#include "tier0/ilogger.h"
#include "filesystem/ifilesystem.h"
#include "utils/interfaces/interfaces.h"

//-----------------------------------------------------------------------------
// A log output into a file
//-----------------------------------------------------------------------------
class CLogOutputFile : public ILogOutput
{
public:
	// ILogOutput interface
	virtual void Print( const logContext_t& context, const char* pMessage ) override;

	TRefPtr<IStreamDataWriter> BeginLoggingToFile( const char* pPath );
	void					   EndLoggingToFile();
	TRefPtr<IStreamDataWriter> GetFile() const;

private:
	TRefPtr<IStreamDataWriter> pFile;
};

#include "tier1/tier1_logoutputs.inl"
