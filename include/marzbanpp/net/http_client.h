#pragma once

#include <curl/curl.h>

#include <string>

#include "http_headers.h"

namespace marzbanpp {

class HttpClient final {
 public:
  struct BasicAuth {
    std::string username;
    std::string password;
  };

  struct Response {
    using Headers = std::vector<std::string>;

    int status_code;
    std::string body;
    Headers headers;
  };

  HttpClient();

  Response Get(
    const std::string& uri,
    const HttpHeaders& headers = {},
    bool follow_location = true) const;

  Response Put(
    const std::string& uri,
    const std::string& payload,
    const HttpHeaders& headers = {},
    bool follow_location = true) const;

  Response Post(
    const std::string& uri,
    const std::string& payload,
    const HttpHeaders& headers = {},
    const std::optional<BasicAuth>& auth = std::nullopt,
    bool follow_location = true) const;

  Response Delete(
    const std::string& uri,
    const std::string& payload,
    const HttpHeaders& headers = {},
    bool follow_location = true) const;
};

}// namespace marzbanpp
