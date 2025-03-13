#include "api.h"
#include "logger.h"

using namespace std::chrono_literals;

std::string FormatResponse(const marzbanpp::Api::Error& error) {
  auto formatted = std::to_string(error.status_code) + '\n';

  for (const auto& header : error.response_headers) {
    formatted += header;
  }

  formatted += '\n' + error.response_body + "\n\n";
  formatted += error.error;
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

    auto result = api->GetHosts();
    LOG_INFO(ExplainResponse(result));

    return EXIT_SUCCESS;
  } catch (const std::exception& ex) {
    printf("error: %s\n", ex.what());
    return EXIT_FAILURE;
  }
}
