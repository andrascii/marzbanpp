#pragma once

#include <chrono>

#include "net/http_client.h"
#include "types/inbounds.h"
#include "types/user.h"
#include "types/users.h"
#include "types/user_usage.h"
#include "types/user_list.h"

namespace marzbanpp {

class Api {
 public:
 using TimePoint = std::chrono::time_point<
    std::chrono::system_clock,
    std::chrono::seconds>;

  using Ptr = std::shared_ptr<Api>;

  enum class RestApiStatusCode {
    kOk = 200,
    kUnauthorized = 401,
    kYouAreNotAllowed = 403,
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

  struct ExpiredUsersParams {
    std::optional<TimePoint> before;
    std::optional<TimePoint> after;
  };

  struct Error {
    std::string response_body;
    std::vector<std::string> response_headers;
    int status_code;
    std::string error;
  };

  template <typename T>
  using Expected = std::expected<T, Error>;

 public:
  static Expected<Ptr> AuthAndCreate(
    const std::string& uri,
    const std::string& username,
    const std::string& password);

  Expected<Inbounds> GetInbounds() const;

  Expected<User> AddUser(const User& user) const;

  Expected<User> GetUser(const std::string& username) const;

  Expected<User> ModifyUser(const std::string& username, const User& modified_user) const;

  HttpClient::Response RemoveUser(const std::string& username) const;

  Expected<User> ResetUserDataUsage(const std::string& username) const;

  Expected<User> RevokeUserSubscription(const std::string& username) const;

  Expected<Users> GetUsers(GetUsersParams params) const;

  HttpClient::Response ResetUsersDataUsage() const;

  Expected<UserUsage> GetUserUsage(const std::string& username, const TimePoint& start, const TimePoint& end = {}) const;

  Expected<User> SetOwner(const std::string& username, const std::string& admin_username) const;

  //!
  //! Can be passed:
  //!   1. 'before' and 'after' together
  //!   2. only one parameter 'before' or 'after'
  //!   3. no one parameter
  //!
  //! In case when we pass nothing - returns a list of all expired users.
  //!
  Expected<UserList> GetExpiredUsers(const ExpiredUsersParams& params = {}) const;

  //!
  //! Can be passed:
  //!   1. 'before' and 'after' together
  //!   2. only one parameter 'before' or 'after'
  //!   3. no one parameter
  //!
  //! In case when we pass nothing - returns a list of all expired users.
  //!
  Expected<UserList> DeleteExpiredUsers(const ExpiredUsersParams& params = {}) const;

 private:
  Api(std::string uri, std::string token_type, std::string access_token);
  virtual ~Api();

 private:
  std::string uri_;
  std::string token_type_;
  std::string access_token_;
};

}// namespace marzbanpp
