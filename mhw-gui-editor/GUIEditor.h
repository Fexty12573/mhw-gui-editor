#pragma once

#include "GUIFile.h"

class GUIEditor {
public:
	GUIEditor();

	void render();
	void open_file();

	[[nodiscard]] GUIFile& get_file() { return m_file; }

private:
	GUIFile m_file;
};

