#pragma once

#include "marzbanpp/net/http_client.h"

namespace marzbanpp {

struct CurlInitializeError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

class CurlError : public std::exception {
 public:
  explicit CurlError(CURLcode error_code) : error_code_{error_code} {}

  const char* what() const noexcept override {
    return curl_easy_strerror(error_code_);
  }

 private:
  CURLcode error_code_;
};

struct MarzbanPanelAuthorizationError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

class FromJsonToObjectError : public std::exception {
 public:
  FromJsonToObjectError(
    const glz::error_ctx& error_ctx,
    const HttpClient::Response& response)
      : response_{response},
        error_explanation_{glz::format_error(error_ctx, response_.body)} {}

  const HttpClient::Response& Response() const noexcept {
    return response_;
  }

  const char* what() const noexcept override {
    return error_explanation_.data();
  }

 private:
  HttpClient::Response response_;
  std::string error_explanation_;
};

class ToObjectFromJsonError : public std::runtime_error {
 public:
  explicit ToObjectFromJsonError(const glz::error_ctx& error_ctx)
      : std::runtime_error{std::string{error_ctx.custom_error_message}} {}

 private:
  std::string error_explanation_;
};

struct UsernameFieldInUserWasNotSet : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct StatusFieldInUserWasNotSet : std::runtime_error {
  using std::runtime_error::runtime_error;
};

class UnexpectedStatusFieldValueInUser : public std::runtime_error {
 public:
  UnexpectedStatusFieldValueInUser(const std::string& actual_value, const std::vector<std::string>& allowed_values)
      : std::runtime_error{"unexpected value '" + actual_value + "', expected values are: " + VectorToString(allowed_values)} {}

 private:
  static std::string VectorToString(const std::vector<std::string>& allowed_values) {
    std::string result = "{";

    for (const auto& value : allowed_values) {
      result += value + ",";
    }

    result.pop_back();
    result += "}";

    return result;
  }
};

}// namespace marzbanpp