#include "logger.h"

namespace {

constexpr auto kLogFileName{"vpn-tg-bot.log"};

struct DailyLogFileName final {
  static spdlog::filename_t calc_filename(
    const spdlog::filename_t& filename,
    const tm& now_tm) {
    using namespace spdlog;

    filename_t basename, ext;
    std::tie(basename, ext) =
      details::file_helper::split_by_extension(filename);

    return fmt::format(
      SPDLOG_FILENAME_T("{}.{:04d}{:02d}{:02d}{}"),
      basename,
      now_tm.tm_year + 1900u,
      now_tm.tm_mon + 1u,
      now_tm.tm_mday,
      ext);
  }
};

using DailyFileSink =
  spdlog::sinks::daily_file_sink<std::mutex, DailyLogFileName>;

}// namespace

std::shared_ptr<spdlog::logger> Logger() noexcept {
  using namespace spdlog;

  static std::once_flag flag;
  const auto name = "logger";

  std::call_once(flag, [name] {
    try {
      const auto file_sink = std::make_shared<DailyFileSink>(kLogFileName, 0, 0);
      const auto instance = std::make_shared<logger>(logger(name, {file_sink}));
      register_logger(instance);
      set_default_logger(instance);
      flush_every(3s);
    } catch (const std::exception& ex) {
      std::cerr
        << "Logger initialization failed: "
        << ex.what()
        << std::endl;
      abort();
    }
  });

  return get(name);
}

std::error_code EnableConsoleLogging() noexcept {
  using namespace spdlog;

  try {
    Logger()->sinks().push_back(std::make_shared<sinks::stdout_color_sink_mt>());
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    return std::make_error_code(std::errc::operation_canceled);
  }

  return std::error_code{};
}

std::error_code DisableConsoleLogging() noexcept {
  using namespace spdlog;

  try {
    Logger()->sinks().clear();
    Logger()->sinks().push_back(std::make_shared<DailyFileSink>(kLogFileName, 0, 0));
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    return std::make_error_code(std::errc::operation_canceled);
  }

  return std::error_code{};
}
