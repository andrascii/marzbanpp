#pragma once

#define LOG_TRACE(...)                            \
  if (spdlog::should_log(spdlog::level::trace)) { \
    SPDLOG_TRACE(__VA_ARGS__);                    \
  }

#define LOG_DEBUG(...)                            \
  if (spdlog::should_log(spdlog::level::debug)) { \
    SPDLOG_DEBUG(__VA_ARGS__);                    \
  }

#define LOG_INFO(...)                            \
  if (spdlog::should_log(spdlog::level::info)) { \
    SPDLOG_INFO(__VA_ARGS__);                    \
  }

#define LOG_WARNING(...)                         \
  if (spdlog::should_log(spdlog::level::warn)) { \
    SPDLOG_WARN(__VA_ARGS__);                    \
  }

#define LOG_ERROR(...)                          \
  if (spdlog::should_log(spdlog::level::err)) { \
    SPDLOG_ERROR(__VA_ARGS__);                  \
  }

#define LOG_CRITICAL(...)                            \
  if (spdlog::should_log(spdlog::level::critical)) { \
    SPDLOG_CRITICAL(__VA_ARGS__);                    \
  }

std::shared_ptr<spdlog::logger> Logger() noexcept;
std::error_code EnableConsoleLogging() noexcept;
std::error_code DisableConsoleLogging() noexcept;