#include "pch.h"
#include "HrException.h"
#include "Console.h"

#include <comdef.h>
#include <fmt/format.h>


HrException::HrException(HRESULT hr, const char* file, int line) : m_hr(hr), m_file(file), m_line(line) {
	m_message = fmt::format(
		"[Error] 0x{:08X} in file {} at line {}\n[Description] {}",
		m_hr,
		m_file,
		m_line,
		get_error_string()
	);

	spdlog::critical(m_message);
}

const char* HrException::what() const noexcept {
	return m_message.c_str();
}

HRESULT HrException::get_error_code() const noexcept {
	return m_hr;
}

std::string HrException::get_error_string() const noexcept {
	return _com_error(m_hr).ErrorMessage();
}
