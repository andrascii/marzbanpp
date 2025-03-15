#include "marzbanpp/api.h"
#include "marzbanpp/types/exceptions.h"
#include "logger.h"

using namespace std::chrono_literals;

std::string FormatResponse(const marzbanpp::HttpClient::Response& response) {
  auto formatted = std::to_string(response.status_code) + '\n';

  for (const auto& header : response.headers) {
    formatted += header;
  }

  formatted += '\n' + response.body + "\n\n";
  return formatted;
}

template <typename T>
std::string ToJson(const T& value) {
  std::string buffer;
  const auto error = glz::write_json(value, buffer);

  if (error) {
    return std::string{error.custom_error_message};
  }

  return glz::prettify_json(buffer);
}

int main() {
  using namespace marzbanpp;

  Logger()->set_level(spdlog::level::trace);

  if (const auto error = EnableConsoleLogging(); error) {
    std::cerr << error.message() << std::endl;
    return EXIT_FAILURE;
  }

  try {
    const auto api = Api::AuthAndCreate("http://localhost:8000", "mb", "123");
    LOG_INFO(ToJson(api->GetHosts()));

    return EXIT_SUCCESS;
  } catch(const marzbanpp::FromJsonToObjectError& ex) {
    LOG_ERROR(FormatResponse(ex.Response()) + ex.what());
  } catch (const std::exception& ex) {
    printf("error: %s\n", ex.what());
    return EXIT_FAILURE;
  }
}
