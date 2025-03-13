#include "api.h"
#include "logger.h"

template <typename T>
std::string ExplainResponse(const marzbanpp::Api::Expected<T>& value) {
  if (value) {
    std::string buffer;
    const auto error = glz::write_json(value, buffer);

    if (error) {
      return error.custom_error_message;
    }

    return glz::prettify_json(buffer);
  } else {
    auto response = std::to_string(value.error().status_code) + '\n';

    for (const auto& header : value.error().response_headers) {
      response += header;
    }

    response += '\n' + value.error().response_body;
    return response;
  }
}

int main() {
  using namespace marzbanpp;

  Logger()->set_level(spdlog::level::trace);

  if (const auto error = EnableConsoleLogging(); error) {
    std::cerr << error.message() << std::endl;
    return EXIT_FAILURE;
  }

  

  return 0;
}
