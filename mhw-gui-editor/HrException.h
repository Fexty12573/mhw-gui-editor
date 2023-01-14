#pragma once

#include "MinWindows.h"

#include <exception>
#include <string>

#ifdef _DEBUG
#define HR_INIT(init) HRESULT _exc_hr = init
#define HR_ASSERT(expr) _exc_hr = expr; if (FAILED(_exc_hr)) throw HrException(_exc_hr, __FILE__, __LINE__)
#else
#define HR_INIT(init)
#define HR_ASSERT(expr) expr
#endif

class HrException : public std::exception {
public:
	HrException(HRESULT hr, const char* file, int line);

	[[nodiscard]] const char* what() const noexcept override;
	[[nodiscard]] HRESULT get_error_code() const noexcept;
	[[nodiscard]] std::string get_error_string() const noexcept;

private:
	HRESULT m_hr;
	const char* m_file;
	int m_line;
	std::string m_message;
};

