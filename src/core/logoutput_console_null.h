#pragma once
#include "core/debug.h"

// A Null log output to a console
class CNullLogOutputConsole : public ILogOutputConsole
{
public:
	// ILogOutput interface
	virtual void SetTextColor( logTextColor_t textColor ) override {}
	virtual void Print( logLevel_t level, const char* pMessage ) override {}

	// ILogOutputConsole interface
	virtual void Show( bool bShowConsole ) override {}
	virtual bool IsShown() const override { return false; }
};