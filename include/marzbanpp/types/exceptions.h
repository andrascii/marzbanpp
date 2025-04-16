#pragma once

#include <stdexcept>

#include "marzbanpp/net/http_client.h"

namespace marzbanpp {

struct MarzbanppError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct CurlInitializeError : MarzbanppError {
  using MarzbanppError::MarzbanppError;
};

class CurlError : public MarzbanppError {
 public:
  explicit CurlError(CURLcode error_code) : MarzbanppError{curl_easy_strerror(error_code)} {}
};

class FromJsonToObjectError : public MarzbanppError {
 public:
  FromJsonToObjectError(
    const glz::error_ctx& error_ctx,
    const HttpClient::Response& response)
      : MarzbanppError{glz::format_error(error_ctx, response.body)},
        response_{response} {}

  const HttpClient::Response& Response() const noexcept {
    return response_;
  }

 private:
  HttpClient::Response response_;
};

class ToObjectFromJsonError : public MarzbanppError {
 public:
  explicit ToObjectFromJsonError(const glz::error_ctx& error_ctx)
      : MarzbanppError{std::string{error_ctx.custom_error_message}} {}

 private:
  std::string error_explanation_;
};

struct UsernameFieldInUserWasNotSet : MarzbanppError {
  using MarzbanppError::MarzbanppError;
};

struct StatusFieldInUserWasNotSet : MarzbanppError {
  using MarzbanppError::MarzbanppError;
};

class UnexpectedStatusFieldValueInUser : public MarzbanppError {
 public:
  UnexpectedStatusFieldValueInUser(const std::string& actual_value, const std::vector<std::string>& allowed_values)
      : MarzbanppError{"unexpected value '" + actual_value + "', expected values are: " + VectorToString(allowed_values)} {}

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

class MarzbanServerResponseError : public MarzbanppError {
 public:
  MarzbanServerResponseError(const HttpClient::Response& response)
      : MarzbanppError{FormatResponse(response)},
        response_{response} {}

  const HttpClient::Response& Response() const noexcept {
    return response_;
  }

 private:
  std::string FormatResponse(const marzbanpp::HttpClient::Response& response) {
    auto formatted = std::to_string(response.status_code) + '\n';

    for (const auto& header : response.headers) {
      formatted += header;
    }

    formatted += '\n' + response.body + "\n\n";
    return formatted;
  }

 private:
  HttpClient::Response response_;
};

}// namespace marzbanpp
