#pragma once

#include "marzbanpp/types/admin_token.h"
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

class IApi {
 public:
  using TimePoint = std::chrono::time_point<
    std::chrono::system_clock,
    std::chrono::seconds>;

  using Ptr = std::shared_ptr<IApi>;

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
  virtual void SetAdminToken(const AdminToken& token) = 0;

  virtual Admin GetCurrentAdmin() const = 0;
  virtual Admin CreateAdmin(const Admin& admin) const = 0;
  virtual Admin ModifyAdmin(const std::string& username, const Admin& admin) const = 0;
  virtual Admin RemoveAdmin(const std::string& username) const = 0;
  virtual Admins GetAdmins(const GetAdminsParams& params = {}) const = 0;

  virtual System GetSystemStats() const = 0;
  virtual Inbounds GetInbounds() const = 0;
  virtual Hosts GetHosts() const = 0;
  virtual Hosts ModifyHosts(const Hosts& hosts) const = 0;

  virtual User AddUser(const User& user) const = 0;
  virtual User GetUser(const std::string& username) const = 0;
  virtual User ModifyUser(const std::string& username, const User& modified_user) const = 0;
  virtual HttpClient::Response RemoveUser(const std::string& username) const = 0;
  virtual User ResetUserDataUsage(const std::string& username) const = 0;
  virtual User RevokeUserSubscription(const std::string& username) const = 0;
  virtual Users GetUsers(const GetUsersParams& params = {}) const = 0;
  virtual HttpClient::Response ResetUsersDataUsage() const = 0;
  virtual UserUsage GetUserUsage(const std::string& username, const TimePoint& start, const TimePoint& end = {}) const = 0;
  virtual User SetOwner(const std::string& username, const std::string& admin_username) const = 0;
  virtual UserList GetExpiredUsers(const ExpiredUsersParams& params = {}) const = 0;
  virtual UserList DeleteExpiredUsers(const ExpiredUsersParams& params = {}) const = 0;

  virtual ~IApi() = default;
};

}// namespace marzbanpp
