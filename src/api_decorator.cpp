#include "marzbanpp/api_decorator.h"

#include "marzbanpp/api.h"
#include "marzbanpp/types/exceptions.h"

namespace {

using namespace marzbanpp;

auto WrapPossiblyUnauthorizedCall(
  const std::string& uri,
  const std::string& username,
  const std::string& password,
  const IApi::Ptr& api,
  const auto& invocable,
  auto&&... args) {
  try {
    return (api.get()->*invocable)(std::forward<decltype(args)>(args)...);
  } catch (const MarzbanServerResponseError& ex) {
    if (ex.Response().status_code != 401) {
      throw;
    }

    api->SetAdminToken(Api::GetAdminToken(uri, username, password));
    return (api.get()->*invocable)(std::forward<decltype(args)>(args)...);
  }
}

}// namespace

namespace marzbanpp {

ApiDecorator::ApiDecorator(std::string uri, std::string username, std::string password)
    : uri_{std::move(uri)},
      username_{std::move(username)},
      password_{std::move(password)} {
  api_ = Api::AuthAndCreate(uri_, username_, password_);
}

void
ApiDecorator::SetAdminToken(const AdminToken& token) {
  api_->SetAdminToken(token);
}

Admin
ApiDecorator::GetCurrentAdmin() const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::GetCurrentAdmin);
}

Admin
ApiDecorator::CreateAdmin(const Admin& admin) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::CreateAdmin, admin);
}

Admin
ApiDecorator::ModifyAdmin(const std::string& username, const Admin& admin) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::ModifyAdmin, username, admin);
}

Admin
ApiDecorator::RemoveAdmin(const std::string& username) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::RemoveAdmin, username);
}

Admins
ApiDecorator::GetAdmins(const GetAdminsParams& params) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::GetAdmins, std::move(params));
}

System
ApiDecorator::GetSystemStats() const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::GetSystemStats);
}

Inbounds
ApiDecorator::GetInbounds() const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::GetInbounds);
}

Hosts
ApiDecorator::GetHosts() const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::GetHosts);
}

Hosts
ApiDecorator::ModifyHosts(const Hosts& hosts) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::ModifyHosts, hosts);
}

User
ApiDecorator::AddUser(const User& user) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::AddUser, user);
}

User
ApiDecorator::GetUser(const std::string& username) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::GetUser, username);
}

User
ApiDecorator::ModifyUser(const std::string& username, const User& modified_user) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::ModifyUser, username, modified_user);
}

HttpClient::Response
ApiDecorator::RemoveUser(const std::string& username) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::RemoveUser, username);
}

User
ApiDecorator::ResetUserDataUsage(const std::string& username) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::ResetUserDataUsage, username);
}

User
ApiDecorator::RevokeUserSubscription(const std::string& username) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::RevokeUserSubscription, username);
}

Users
ApiDecorator::GetUsers(const GetUsersParams& params) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::GetUsers, std::move(params));
}

HttpClient::Response
ApiDecorator::ResetUsersDataUsage() const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::ResetUsersDataUsage);
}

UserUsage
ApiDecorator::GetUserUsage(const std::string& username, const TimePoint& start, const TimePoint& end) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::GetUserUsage, username, start, end);
}

User
ApiDecorator::SetOwner(const std::string& username, const std::string& admin_username) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::SetOwner, username, admin_username);
}

UserList
ApiDecorator::GetExpiredUsers(const ExpiredUsersParams& params) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::GetExpiredUsers, params);
}

UserList
ApiDecorator::DeleteExpiredUsers(const ExpiredUsersParams& params) const {
  return WrapPossiblyUnauthorizedCall(uri_, username_, password_, api_, &IApi::DeleteExpiredUsers, params);
}

}// namespace marzbanpp
