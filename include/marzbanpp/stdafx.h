#pragma once

// C/C++
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

// glaze
#include <glaze/glaze.hpp>
#include <glaze/json/prettify.hpp>

// spdlog

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4005)
#endif

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/details/file_helper.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#if defined(_WIN32)
#pragma warning(pop)
#endif

// curl
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/header.h>

using namespace std::literals;
