#pragma once

#include "types/inbounds.h"
#include "types/user.h"
#include "types/users.h"

namespace marzbanpp {

class Api {
 public:
  using Ptr = std::shared_ptr<Api>;

  enum class RestApiStatusCode {
    kOk = 200,
    kYouAreNotAllowed = 403, // possibly because requested was unauthorized
    kUserNotFound = 404,
    kValidationError = 422,
  };

  struct GetUsersParams {
    std::optional<uint64_t> offset;
    std::optional<uint64_t> limit;
    std::optional<std::vector<std::string>> username;
    std::optional<std::string> status;
    std::optional<std::string> sort;
  };

  struct Error {
    std::string response_body;
    std::vector<std::string> response_headers;
    int status_code;
    std::string error;
  };

  struct Detail {
    std::string detail;
  };

  template <typename T>
  using Expected = std::expected<T, Error>;

 public:
  static Ptr AuthAndCreate(
    const std::string& uri,
    const std::string& username,
    const std::string& password);

  Expected<Inbounds> GetInbounds() const;

  Expected<User> AddUser(const User& user) const;

  Expected<User> GetUser(const std::string& username) const;

  Expected<User> ModifyUser(const std::string& username, const User& modified_user) const;

  RestApiStatusCode RemoveUser(const std::string& username) const;

  Expected<User> ResetUserDataUsage(const std::string& username) const;

  Expected<Users> GetUsers(GetUsersParams params) const;

 private:
  Api(std::string uri, std::string token_type, std::string access_token);
  virtual ~Api();

 private:
  std::string uri_;
  std::string token_type_;
  std::string access_token_;
};

}// namespace marzbanpp
