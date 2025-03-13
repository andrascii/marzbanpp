#pragma once

namespace marzbanpp {

class HttpHeaders final {
 public:
  HttpHeaders();
  ~HttpHeaders();

  void Add(const std::string& name, const std::string& value);

  curl_slist* Get() const noexcept;

 private:
  curl_slist* headers_;
};

}// namespace marzbanpp
