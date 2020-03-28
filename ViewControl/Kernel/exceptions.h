#pragma once

#include <iostream>
#include <windows.h>

class Exception
{
	friend std::ostream& operator << (std::ostream &lhs, const Exception &exception);

private:
	std::string _reason;

public:

	Exception(std::string reason = "Exception") : _reason(reason) 
	{
	}

	Exception(const Exception &exception) : _reason(exception._reason) 
	{
	}

	Exception& operator = (const Exception &exception)
	{
		if (this == &exception) return *this;
		_reason = exception._reason;
		return *this;
	}

	void ShowReason(HWND parent = 0) const
	{
		MessageBoxA(parent, _reason.c_str(), "Antlers - Exception occured", 0);
	}

	virtual ~Exception() 
	{
		_reason.clear();
	}

};

inline std::ostream& operator << (std::ostream &lhs, const Exception &rhs)
{
	return lhs << rhs._reason;
}
