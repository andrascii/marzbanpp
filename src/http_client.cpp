#include "marzbanpp/net/http_client.h"

#include "marzbanpp/types/exceptions.h"
#include "marzbanpp/finally.h"
#include "marzbanpp/net/http_headers.h"

namespace {

using namespace marzbanpp;

enum ErrorCodes {
  kErrorCreatingCurlHandle = -1,
};

size_t WriteResponseBodyCallback(void* buffer, size_t size, size_t nmemb, void* user_data) {
  size_t total_size = size * nmemb;

  HttpClient::Response* response = static_cast<HttpClient::Response*>(user_data);
  response->body.append(static_cast<const char*>(buffer), total_size);

  return total_size;
}

size_t WriteResponseHeadersCallback(void* buffer, size_t size, size_t nmemb, void* user_data) {
  size_t total_size = size * nmemb;

  HttpClient::Response* response = static_cast<HttpClient::Response*>(user_data);
  response->headers.push_back(static_cast<const char*>(buffer));

  return total_size;
}

}// namespace

namespace marzbanpp {

HttpClient::HttpClient() {}

HttpClient::Response
HttpClient::Get(
  const std::string& uri,
  const HttpHeaders& headers,
  bool follow_location) const {
  CURL* easy = curl_easy_init();

  if (!easy) {
    throw CurlInitializeError{"curl_easy_init() failed"};
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
    throw CurlError{result};
  }

  result = curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response.status_code);

  if (result != CURLE_OK) {
    throw CurlError{result};
  }

  return response;
}

HttpClient::Response
HttpClient::Put(
  const std::string& uri,
  const std::string& payload,
  const HttpHeaders& headers,
  bool follow_location) const {
  CURL* easy = curl_easy_init();

  if (!easy) {
    throw CurlInitializeError{"curl_easy_init() failed"};
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
    throw CurlError{result};
  }

  result = curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response.status_code);

  if (result != CURLE_OK) {
    throw CurlError{result};
  }

  return response;
}

HttpClient::Response
HttpClient::Post(
  const std::string& uri,
  const std::string& payload,
  const HttpHeaders& headers,
  bool follow_location) const {
  CURL* easy = curl_easy_init();

  if (!easy) {
    throw CurlInitializeError{"curl_easy_init() failed"};
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
    throw CurlError{result};
  }

  result = curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response.status_code);

  if (result != CURLE_OK) {
    throw CurlError{result};
  }

  return response;
}

HttpClient::Response
HttpClient::Delete(
  const std::string& uri,
  const std::string& payload,
  const HttpHeaders& headers,
  bool follow_location) const {
  CURL* easy = curl_easy_init();

  if (!easy) {
    throw CurlInitializeError{"curl_easy_init() failed"};
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
    throw CurlError{result};
  }

  result = curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response.status_code);

  if (result != CURLE_OK) {
    throw CurlError{result};
  }

  return response;
}

}// namespace marzbanpp
