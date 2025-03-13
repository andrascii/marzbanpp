#include "http_client.h"

#include "finally.h"
#include "http_headers.h"

namespace {

enum ErrorCodes {
  kErrorCreatingCurlHandle = -1,
};

}

namespace marzbanpp {

HttpClient::Error::Error(int ec) : error_code{ec} {}

std::string HttpClient::Error::Description() const noexcept {
  switch (error_code) {
    case ErrorCodes::kErrorCreatingCurlHandle: {
      return "curl_easy_init() failed"s;
    }
    default: {
      return curl_easy_strerror(static_cast<CURLcode>(error_code));
    }
  }
};

HttpClient::HttpClient() {}

HttpClient::ExpectedResponse HttpClient::Get(
  const std::string& uri,
  const HttpHeaders& headers,
  bool follow_location) const noexcept {
  CURL* easy = curl_easy_init();

  if (!easy) {
    const auto error = Error{ErrorCodes::kErrorCreatingCurlHandle};
    return std::unexpected{error};
  }

  Finally _{[easy] noexcept { curl_easy_cleanup(easy); }};

  Response response;

  curl_easy_setopt(easy, CURLOPT_URL, uri.data());
  curl_easy_setopt(easy, CURLOPT_HTTPHEADER, headers.Get());
  curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, WriteResponseBodyCallback);
  curl_easy_setopt(easy, CURLOPT_HEADERFUNCTION, WriteResponseHeadersCallback);
  curl_easy_setopt(easy, CURLOPT_HEADERDATA, &response);
  curl_easy_setopt(easy, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(easy, CURLOPT_FOLLOWLOCATION, follow_location);

  CURLcode result = curl_easy_perform(easy);

  if (result != CURLE_OK) {
    const auto error = Error{result};
    return std::unexpected{error};
  }

  result = curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response.status_code);

  if (result != CURLE_OK) {
    const auto error = Error{result};
    return std::unexpected{error};
  }

  return response;
}

HttpClient::ExpectedResponse
HttpClient::Put(
  const std::string& uri,
  const std::string& payload,
  const HttpHeaders& headers,
  bool follow_location) const noexcept {
  CURL* easy = curl_easy_init();

  if (!easy) {
    const auto error = Error{ErrorCodes::kErrorCreatingCurlHandle};
    return std::unexpected{error};
  }

  Finally _{[easy] noexcept { curl_easy_cleanup(easy); }};

  Response response;

  curl_easy_setopt(easy, CURLOPT_URL, uri.data());
  curl_easy_setopt(easy, CURLOPT_CUSTOMREQUEST, "PUT");
  curl_easy_setopt(easy, CURLOPT_HTTPHEADER, headers.Get());
  curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, WriteResponseBodyCallback);
  curl_easy_setopt(easy, CURLOPT_HEADERFUNCTION, WriteResponseHeadersCallback);
  curl_easy_setopt(easy, CURLOPT_HEADERDATA, &response);
  curl_easy_setopt(easy, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(easy, CURLOPT_POSTFIELDS, payload.data());
  curl_easy_setopt(easy, CURLOPT_FOLLOWLOCATION, follow_location);

  CURLcode result = curl_easy_perform(easy);

  if (result != CURLE_OK) {
    const auto error = Error{result};
    return std::unexpected{error};
  }

  result = curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response.status_code);

  if (result != CURLE_OK) {
    const auto error = Error{result};
    return std::unexpected{error};
  }

  return response;
}

HttpClient::ExpectedResponse
HttpClient::Post(
  const std::string& uri,
  const std::string& payload,
  const HttpHeaders& headers,
  bool follow_location) const noexcept {
  CURL* easy = curl_easy_init();

  if (!easy) {
    const auto error = Error{ErrorCodes::kErrorCreatingCurlHandle};
    return std::unexpected{error};
  }

  Finally _{[easy] noexcept { curl_easy_cleanup(easy); }};

  Response response;

  curl_easy_setopt(easy, CURLOPT_URL, uri.data());
  curl_easy_setopt(easy, CURLOPT_HTTPHEADER, headers.Get());
  curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, WriteResponseBodyCallback);
  curl_easy_setopt(easy, CURLOPT_HEADERFUNCTION, WriteResponseHeadersCallback);
  curl_easy_setopt(easy, CURLOPT_HEADERDATA, &response);
  curl_easy_setopt(easy, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(easy, CURLOPT_POSTFIELDS, payload.data());
  curl_easy_setopt(easy, CURLOPT_FOLLOWLOCATION, follow_location);

  CURLcode result = curl_easy_perform(easy);

  if (result != CURLE_OK) {
    const auto error = Error{result};
    return std::unexpected{error};
  }

  result = curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response.status_code);

  if (result != CURLE_OK) {
    const auto error = Error{result};
    return std::unexpected{error};
  }

  return response;
}

HttpClient::ExpectedResponse
HttpClient::Delete(
  const std::string& uri,
  const std::string& payload,
  const HttpHeaders& headers,
  bool follow_location) const noexcept {
  CURL* easy = curl_easy_init();

  if (!easy) {
    const auto error = Error{ErrorCodes::kErrorCreatingCurlHandle};
    return std::unexpected{error};
  }

  Finally _{[easy] noexcept { curl_easy_cleanup(easy); }};

  Response response;

  curl_easy_setopt(easy, CURLOPT_URL, uri.data());
  curl_easy_setopt(easy, CURLOPT_CUSTOMREQUEST, "DELETE");
  curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, WriteResponseBodyCallback);
  curl_easy_setopt(easy, CURLOPT_HEADERFUNCTION, WriteResponseHeadersCallback);
  curl_easy_setopt(easy, CURLOPT_HEADERDATA, &response);
  curl_easy_setopt(easy, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(easy, CURLOPT_HTTPHEADER, headers.Get());
  curl_easy_setopt(easy, CURLOPT_POSTFIELDS, payload.data());
  curl_easy_setopt(easy, CURLOPT_FOLLOWLOCATION, follow_location);

  CURLcode result = curl_easy_perform(easy);

  if (result != CURLE_OK) {
    const auto error = Error{result};
    return std::unexpected{error};
  }

  result = curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response.status_code);

  if (result != CURLE_OK) {
    const auto error = Error{result};
    return std::unexpected{error};
  }

  return response;
}

size_t HttpClient::WriteResponseBodyCallback(void* buffer, size_t size, size_t nmemb, void* user_data) {
  size_t total_size = size * nmemb;

  Response* response = static_cast<Response*>(user_data);
  response->body.append(static_cast<const char*>(buffer), total_size);

  return total_size;
}

size_t HttpClient::WriteResponseHeadersCallback(void* buffer, size_t size, size_t nmemb, void* user_data) {
  size_t total_size = size * nmemb;

  Response* response = static_cast<Response*>(user_data);
  response->headers.push_back(static_cast<const char*>(buffer));

  return total_size;
}

}// namespace marzbanpp
