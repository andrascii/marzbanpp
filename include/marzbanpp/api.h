#pragma once

#include <chrono>

#include "net/http_client.h"
#include "types/admins.h"
#include "types/hosts.h"
#include "types/inbounds.h"
#include "types/system.h"
#include "types/user.h"
#include "types/user_list.h"
#include "types/user_usage.h"
#include "types/users.h"

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

  struct GetAdminsParams {
    std::optional<uint64_t> offset;
    std::optional<uint64_t> limit;
    std::optional<std::vector<std::string>> username;
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

 public:
  static Ptr AuthAndCreate(
    const std::string& uri,
    const std::string& username,
    const std::string& password);

  Admin GetCurrentAdmin() const;
  Admin CreateAdmin(const Admin& admin) const;
  Admin ModifyAdmin(const std::string& username, const Admin& admin) const;
  Admin RemoveAdmin(const std::string& username) const;
  Admins GetAdmins(const GetAdminsParams& params = {}) const;

  System GetSystemStats() const;
  Inbounds GetInbounds() const;
  Hosts GetHosts() const;
  Hosts ModifyHosts(const Hosts& hosts) const;

  User AddUser(const User& user) const;
  User GetUser(const std::string& username) const;
  User ModifyUser(const std::string& username, const User& modified_user) const;
  HttpClient::Response RemoveUser(const std::string& username) const;
  User ResetUserDataUsage(const std::string& username) const;
  User RevokeUserSubscription(const std::string& username) const;
  Users GetUsers(GetUsersParams params = {}) const;
  HttpClient::Response ResetUsersDataUsage() const;
  UserUsage GetUserUsage(const std::string& username, const TimePoint& start, const TimePoint& end = {}) const;
  User SetOwner(const std::string& username, const std::string& admin_username) const;
  UserList GetExpiredUsers(const ExpiredUsersParams& params = {}) const;
  UserList DeleteExpiredUsers(const ExpiredUsersParams& params = {}) const;

 private:
  Api(std::string uri, std::string token_type, std::string access_token);
  virtual ~Api();

 private:
  std::string uri_;
  std::string token_type_;
  std::string access_token_;
};

}// namespace marzbanpp
