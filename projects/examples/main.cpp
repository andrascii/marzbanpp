#include "api.h"
#include "logger.h"

using namespace std::chrono_literals;

std::string FormatResponse(const marzbanpp::Api::Error& error) {
  auto formatted = std::to_string(error.status_code) + '\n';

  for (const auto& header : error.response_headers) {
    formatted += header;
  }

  formatted += '\n' + error.response_body;
  return formatted;
}

template <typename T>
std::string ExplainResponse(const marzbanpp::Api::Expected<T>& value) {
  if (value) {
    std::string buffer;
    const auto error = glz::write_json(value, buffer);

    if (error) {
      return std::string{error.custom_error_message};
    }

    return glz::prettify_json(buffer);
  }

  return FormatResponse(value.error());
}

std::chrono::time_point<std::chrono::system_clock> GetStartTimeOfToday() {
  using system_clock = std::chrono::system_clock;
  std::time_t t_now = std::chrono::system_clock::to_time_t(system_clock::now());// Получаем текущее время в виде std::time_t
  std::tm tmp = *localtime(&t_now);                                             // Конвертируем в std::tm.
  tmp.tm_hour = 0;                                                              // Обнуляем все поля времени.
  tmp.tm_min = 0;
  tmp.tm_sec = 0;
  std::time_t t_start_time_of_day = std::mktime(&tmp);// Делаем обратные преобразования.
  return system_clock::from_time_t(t_start_time_of_day);
}

int main() {
  using namespace marzbanpp;

  Logger()->set_level(spdlog::level::trace);

  if (const auto error = EnableConsoleLogging(); error) {
    std::cerr << error.message() << std::endl;
    return EXIT_FAILURE;
  }

  try {
    const auto expected_api = Api::AuthAndCreate("http://localhost:8000", "mb", "123");

    if (!expected_api) {
      LOG_ERROR("error initializing marzbanpp::Api: {}", FormatResponse(expected_api.error()));
      return EXIT_FAILURE;
    }

    const auto& api = *expected_api;
    const auto user_list = api->GetExpiredUsers();
    LOG_INFO(ExplainResponse(user_list));

    const auto resp = api->ResetUsersDataUsage();

    auto tp = GetStartTimeOfToday();
    tp = tp - 86400s * 7;
    const auto andrey_usage = api->GetUserUsage("Andrey",  std::chrono::time_point_cast<std::chrono::seconds>(tp));
    LOG_INFO(ExplainResponse(andrey_usage));

    const auto user_list2 = api->DeleteExpiredUsers();
    LOG_INFO(ExplainResponse(user_list2));

    return EXIT_SUCCESS;
  } catch (const std::exception& ex) {
    printf("error: %s\n", ex.what());
    return EXIT_FAILURE;
  }
}
