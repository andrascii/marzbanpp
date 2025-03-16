#include "marzbanpp/api.h"

#include "marzbanpp/net/http_client.h"
#include "marzbanpp/net/http_headers.h"
#include "marzbanpp/types/exceptions.h"
#include "marzbanpp/types/user.h"

namespace {

using namespace marzbanpp;

[[maybe_unused]] auto RestApiStatusCodeDescription(Api::RestApiStatusCode error) noexcept {
  switch (error) {
    case Api::RestApiStatusCode::kOk: return "Ok"s;
    case Api::RestApiStatusCode::kUnauthorized: return "Unauthorized request - authorize and repeat"s;
    case Api::RestApiStatusCode::kYouAreNotAllowed: return "You're not allowed"s;
    case Api::RestApiStatusCode::kUserNotFound: return "User not found"s;
    case Api::RestApiStatusCode::kValidationError: return "Validation error"s;
    default: return "Unknown error code: "s + std::to_string(static_cast<int>(error));
  }
};

template <typename T>
T ParseResponse(const HttpClient::Response& response) {
  if (response.status_code != static_cast<int>(Api::RestApiStatusCode::kOk)) {
    throw MarzbanServerResponseError{response};
  }

  const auto parsed = glz::read_json<T>(response.body);

  if (parsed) {
    return *parsed;
  }

  throw FromJsonToObjectError{parsed.error(), response};
}

}// namespace

namespace marzbanpp {

struct AdminToken {
  std::string access_token;
  std::string token_type;
};

Api::Ptr
Api::AuthAndCreate(const std::string& uri, const std::string& username, const std::string& password) {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/x-www-form-urlencoded");

  const auto post_data = std::format("username={}&password={}", username, password);

  HttpClient http_client;
  const auto response = http_client.Post(uri + "/api/admin/token", post_data, headers);

  auto admin_token = ParseResponse<AdminToken>(response);

  struct MakeSharedEnabler : Api {
    MakeSharedEnabler(std::string uri, AdminToken token)
        : Api(
            std::move(uri),
            std::move(token.token_type),
            std::move(token.access_token)) {}
  };

  return std::make_shared<MakeSharedEnabler>(uri, std::move(admin_token));
}

Admin Api::GetCurrentAdmin() const {
  HttpHeaders headers;
  HttpClient http_client;

  headers.Add("Authorization", token_type_ + " " + access_token_);
  const auto response = http_client.Get(uri_ + "/api/admin"s, headers);

  return ParseResponse<Admin>(response);
}

Admin Api::CreateAdmin(const Admin& admin) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string create_admin_request;
  const auto error_ctx = glz::write_json(admin, create_admin_request);

  if (error_ctx) {
    throw ToObjectFromJsonError{error_ctx};
  }

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/admin"s, create_admin_request, headers);

  return ParseResponse<Admin>(response);
}

Admin Api::ModifyAdmin(const std::string& username, const Admin& admin) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string modify_admin_request;

  const auto error_ctx = glz::write_json(admin, modify_admin_request);

  if (error_ctx) {
    throw ToObjectFromJsonError{error_ctx};
  }

  HttpClient http_client;
  const auto response = http_client.Put(uri_ + "/api/admin/"s + username, modify_admin_request, headers);

  return ParseResponse<Admin>(response);
}

Admin Api::RemoveAdmin(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Delete(uri_ + "/api/admin/"s + username, {}, headers);

  return ParseResponse<Admin>(response);
}

Admins
Api::GetAdmins(const GetAdminsParams& params) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/x-www-form-urlencoded");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string query;
  std::vector<std::string> data;

  if (params.limit) {
    data.push_back("limit=" + std::to_string(*params.limit));
  }

  if (params.offset) {
    data.push_back("offset=" + std::to_string(*params.offset));
  }

  if (params.username && !params.username->empty()) {
    for (const auto& username : *params.username) {
      data.push_back("username=" + username);
    }
  }

  for (const auto& query_element : data) {
    query += query_element + "&";
  }

  query.pop_back();

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/admins/?" + query, headers);

  return ParseResponse<Admins>(response);
}

System
Api::GetSystemStats() const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/system/"s, headers);

  return ParseResponse<System>(response);
}

Inbounds
Api::GetInbounds() const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/inbounds/"s, headers);

  if (response.status_code != static_cast<int>(RestApiStatusCode::kOk)) {
    throw MarzbanServerResponseError{response};
  }

  return ParseResponse<Inbounds>(response);
}

Hosts Api::GetHosts() const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/hosts/"s, headers);

  return ParseResponse<Hosts>(response);
}

Hosts Api::ModifyHosts(const Hosts& hosts) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string modify_hosts_request;
  const auto error_ctx = glz::write_json(hosts, modify_hosts_request);

  if (error_ctx) {
    throw ToObjectFromJsonError{error_ctx};
  }

  HttpClient http_client;
  const auto response = http_client.Put(uri_ + "/api/hosts/"s, modify_hosts_request, headers);

  return ParseResponse<Hosts>(response);
}

User Api::AddUser(const User& user) const {
  if (!user.username.has_value()) {
    throw UsernameFieldInUserWasNotSet{"'username' field must be set"};
  }

  if (!user.status.has_value()) {
    throw StatusFieldInUserWasNotSet{"'status' field must be set"};
  }

  const auto is_valid_status =
    (*user.status == status_values::kActive || *user.status == status_values::kOnHold);

  if (!is_valid_status) {
    const auto allowed_values = std::vector{
      std::string{status_values::kActive},
      std::string{status_values::kOnHold}};

    throw UnexpectedStatusFieldValueInUser{*user.status, allowed_values};
  }

  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string json_request;
  const auto error_ctx = glz::write_json(user, json_request);

  if (error_ctx) {
    throw ToObjectFromJsonError{error_ctx};
  }

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/user/"s, json_request, headers);

  return ParseResponse<User>(response);
}

User Api::GetUser(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/user/"s + username, headers);

  if (response.status_code != static_cast<int>(RestApiStatusCode::kOk)) {
    throw MarzbanServerResponseError{response};
  }

  return ParseResponse<User>(response);
}

User Api::ModifyUser(const std::string& username, const User& modified_user) const {
  if (!modified_user.username) {
    throw UsernameFieldInUserWasNotSet{"'username' field must be set"};
  }

  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string json_request;
  const auto error_ctx = glz::write_json(modified_user, json_request);

  if (error_ctx) {
    throw ToObjectFromJsonError{error_ctx};
  }

  HttpClient http_client;
  const auto response = http_client.Put(uri_ + "/api/user/"s + username, json_request, headers);

  return ParseResponse<User>(response);
}

HttpClient::Response
Api::RemoveUser(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Delete(uri_ + "/api/user/"s + username, {}, headers);

  return response;
}

User Api::ResetUserDataUsage(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/user/"s + username + "/reset", {}, headers);

  return ParseResponse<User>(response);
}

User Api::RevokeUserSubscription(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/user/"s + username + "/revoke_sub", {}, headers);

  return ParseResponse<User>(response);
}

Users Api::GetUsers(GetUsersParams params) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/x-www-form-urlencoded");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string query;
  std::vector<std::string> data;

  if (params.limit) {
    data.push_back("limit=" + std::to_string(*params.limit));
  }

  if (params.offset) {
    data.push_back("offset=" + std::to_string(*params.offset));
  }

  if (params.sort) {
    data.push_back("sort=" + *params.sort);
  }

  if (params.status) {
    data.push_back("status=" + *params.status);
  }

  if (params.username && !params.username->empty()) {
    for (const auto& username : *params.username) {
      data.push_back("username=" + username);
    }
  }

  for (const auto& query_element : data) {
    query += query_element + "&";
  }

  query.pop_back();

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/users/?" + query, headers);

  return ParseResponse<Users>(response);
}

HttpClient::Response
Api::ResetUsersDataUsage() const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/users/reset"s, {}, headers);

  if (response.status_code != static_cast<int>(RestApiStatusCode::kOk)) {
    throw MarzbanServerResponseError{response};
  }

  return response;
}

UserUsage
Api::GetUserUsage(const std::string& username, const TimePoint& start, const TimePoint& end) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/x-www-form-urlencoded");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  auto query = "start=" + std::format("{:%Y-%m-%dT%H:%M:%S}", start);

  if (end != TimePoint{}) {
    query += "&end=" + std::format("{:%Y-%m-%dT%H:%M:%S}", end);
  }

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/user/"s + username + "/usage/?" + query, headers);

  return ParseResponse<UserUsage>(response);
}

User Api::SetOwner(const std::string& username, const std::string& admin_username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Put(uri_ + "/api/user/"s + username + "/set-owner/?admin_username=" + admin_username, {}, headers);

  return ParseResponse<User>(response);
}

UserList
Api::GetExpiredUsers(const ExpiredUsersParams& params) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string query;

  if (params.before) {
    query = "?expired_before=" + std::format("{:%Y-%m-%dT%H:%M:%S}", *params.before);
  }

  if (params.after) {
    if (!query.empty()) {
      query += '&';
    }

    query = "expired_after=" + std::format("{:%Y-%m-%dT%H:%M:%S}", *params.after);
  }

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/users/expired/"s + query, headers);

  return ParseResponse<UserList>(response);
}

UserList
Api::DeleteExpiredUsers(const ExpiredUsersParams& params) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string query;

  if (params.before) {
    query = "?expired_before=" + std::format("{:%Y-%m-%dT%H:%M:%S}", *params.before);
  }

  if (params.after) {
    if (!query.empty()) {
      query += '&';
    }

    query = "expired_after=" + std::format("{:%Y-%m-%dT%H:%M:%S}", *params.after);
  }

  HttpClient http_client;
  const auto response = http_client.Delete(uri_ + "/api/users/expired/"s + query, {}, headers);

  return ParseResponse<UserList>(response);
}

Api::Api(std::string uri, std::string token_type, std::string access_token)
    : uri_{std::move(uri)},
      token_type_{std::move(token_type)},
      access_token_{std::move(access_token)} {}

Api::~Api() = default;

}// namespace marzbanpp
