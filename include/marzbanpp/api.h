#pragma once

#include "marzbanpp/iapi.h"
#include "marzbanpp/types/admin_token.h"

namespace marzbanpp {

class Api : public IApi {
 public:
  static AdminToken GetAdminToken(
    const std::string& uri,
    const std::string& username,
    const std::string& password);

  static Ptr AuthAndCreate(
    const std::string& uri,
    const std::string& username,
    const std::string& password);

  void SetAdminToken(const AdminToken& token) override;

  Admin GetCurrentAdmin() const override;
  Admin CreateAdmin(const Admin& admin) const override;
  Admin ModifyAdmin(const std::string& username, const Admin& admin) const override;
  Admin RemoveAdmin(const std::string& username) const override;
  Admins GetAdmins(const GetAdminsParams& params = {}) const override;

  System GetSystemStats() const override;
  Inbounds GetInbounds() const override;
  Hosts GetHosts() const override;
  Hosts ModifyHosts(const Hosts& hosts) const override;

  User AddUser(const User& user) const override;
  User GetUser(const std::string& username) const override;
  User ModifyUser(const std::string& username, const User& modified_user) const override;
  HttpClient::Response RemoveUser(const std::string& username) const override;
  User ResetUserDataUsage(const std::string& username) const override;
  User RevokeUserSubscription(const std::string& username) const override;
  Users GetUsers(const GetUsersParams& params = {}) const override;
  HttpClient::Response ResetUsersDataUsage() const override;
  UserUsage GetUserUsage(const std::string& username, const TimePoint& start, const TimePoint& end = {}) const override;
  User SetOwner(const std::string& username, const std::string& admin_username) const override;
  UserList GetExpiredUsers(const ExpiredUsersParams& params = {}) const override;
  UserList DeleteExpiredUsers(const ExpiredUsersParams& params = {}) const override;

 private:
  Api(std::string uri, std::string token_type, std::string access_token);

 private:
  std::string uri_;
  std::string token_type_;
  std::string access_token_;
};

}// namespace marzbanpp
