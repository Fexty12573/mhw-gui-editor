#pragma once

#include "GUIFile.h"

#include <string>

class GUIEditor {
public:
	GUIEditor();

	void render();
	void open_file();

	[[nodiscard]] GUIFile& get_file() { return m_file; }

private:
	GUIFile m_file;
};

