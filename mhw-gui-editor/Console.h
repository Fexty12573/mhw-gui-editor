#pragma once

#include <spdlog/spdlog.h>

#include <memory>

class Console {
public:
    static const std::shared_ptr<Console>& get();

    void initialize();
    void shutdown();

    [[nodiscard]] auto get_logger() const { return m_logger; }

private:
    static std::shared_ptr<Console> m_instance;

    std::shared_ptr<spdlog::logger> m_logger;
};

