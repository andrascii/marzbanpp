#include "marzbanpp/api.h"
#include "marzbanpp/types/exceptions.h"

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

  try {
    const auto api = Api::AuthAndCreate("http://localhost:8000", "mb", "123");
    std::cout << ToJson(api->GetHosts()) << std::endl;

    return EXIT_SUCCESS;
  } catch(const marzbanpp::FromJsonToObjectError& ex) {
    printf("error: %s\n", (FormatResponse(ex.Response()) + ex.what()).data());
  } catch (const std::exception& ex) {
    printf("error: %s\n", ex.what());
    return EXIT_FAILURE;
  }
}
