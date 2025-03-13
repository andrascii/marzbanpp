#include "http_headers.h"

namespace marzbanpp {

HttpHeaders::HttpHeaders()
    : headers_{nullptr} {
}

HttpHeaders::~HttpHeaders() {
  curl_slist_free_all(headers_);
}

void HttpHeaders::Add(const std::string& name, const std::string& value) {
  headers_ = curl_slist_append(headers_, (name + ": " + value).data());

  if (!headers_) {
    throw std::runtime_error("Cannot add header");
  }
}

curl_slist* HttpHeaders::Get() const noexcept {
  return headers_;
}

}// namespace marzbanpp
