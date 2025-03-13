#pragma once

#include <curl/curl.h>

#include <string>

#include "http_headers.h"

namespace marzbanpp {

class HttpClient final {
 public:
  class Error {
   public:
    explicit Error(int ec);
    std::string Description() const noexcept;

   private:
    int error_code;
  };

  struct Response {
    using Headers = std::vector<std::string>;

    int status_code;
    std::string body;
    Headers headers;
  };

  using ExpectedResponse = std::expected<Response, Error>;

  HttpClient();

  ExpectedResponse Get(
    const std::string& uri,
    const HttpHeaders& headers = {},
    bool follow_location = true) const noexcept;

  ExpectedResponse Put(
    const std::string& uri,
    const std::string& payload,
    const HttpHeaders& headers = {},
    bool follow_location = true) const noexcept;

  ExpectedResponse Post(
    const std::string& uri,
    const std::string& payload,
    const HttpHeaders& headers = {},
    bool follow_location = true) const noexcept;

  ExpectedResponse Delete(
    const std::string& uri,
    const std::string& payload,
    const HttpHeaders& headers = {},
    bool follow_location = true) const noexcept;

 private:
  static size_t WriteResponseBodyCallback(void* contents, size_t size, size_t nmemb, void* user_data);
  static size_t WriteResponseHeadersCallback(void* contents, size_t size, size_t nmemb, void* user_data);
};

}// namespace marzbanpp
