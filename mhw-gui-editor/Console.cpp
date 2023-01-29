#include "pch.h"
#include "Console.h"

#include <corecrt_io.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<Console> Console::m_instance = std::make_shared<Console>();

const std::shared_ptr<Console>& Console::get() {
    return m_instance;
}

void Console::initialize() {
    AllocConsole();

    (void)freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
    (void)freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    (void)freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);

    m_logger = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(m_logger);
}

void Console::shutdown() {
    m_logger.reset();
    spdlog::drop("console");
    FreeConsole();
}
