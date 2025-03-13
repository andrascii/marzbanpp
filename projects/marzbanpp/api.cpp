#include "api.h"
#include <glaze/json/prettify.hpp>
#include <stdexcept>

#include "net/http_client.h"
#include "net/http_headers.h"
#include "types/user.h"

namespace {

using namespace marzbanpp;

auto RestApiStatusCodeDescription(Api::RestApiStatusCode error) noexcept {
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
Api::Expected<T> ParseResponse(const HttpClient::Response& response) {
  const auto parsed_json = glz::read_json<T>(response.body);

  if (parsed_json) {
    return *parsed_json;
  }

  const auto error = Api::Error{
    .response_body = response.body,
    .response_headers = response.headers,
    .status_code = response.status_code,
    .error = fmt::format(fmt::runtime("parsing JSON failed: {}"), glz::format_error(parsed_json, response.body))};

  return std::unexpected{error};
}

}// namespace

namespace marzbanpp {

struct AdminToken {
  std::string access_token;
  std::string token_type;
};

Api::Expected<Api::Ptr>
Api::AuthAndCreate(const std::string& uri, const std::string& username, const std::string& password) {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/x-www-form-urlencoded");

  const auto post_data = std::format("username={}&password={}", username, password);

  HttpClient http_client;
  const auto response = http_client.Post(uri + "/api/admin/token", post_data, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("error retrieving admin token for {}: {}"),
        username,
        response.error().Description())};
  }

  if (response->status_code != static_cast<int>(RestApiStatusCode::kOk)) {
    throw std::runtime_error{
      std::format(
        "error retrieving admin token for {}: {}",
        username,
        RestApiStatusCodeDescription(static_cast<RestApiStatusCode>(response->status_code)))};
  }

  auto admin_token = ParseResponse<AdminToken>(*response);

  if (!admin_token) {
    return std::unexpected{admin_token.error()};
  }

  struct MakeSharedEnabler : Api {
    MakeSharedEnabler(std::string uri, AdminToken token)
        : Api(
            std::move(uri),
            std::move(token.token_type),
            std::move(token.access_token)) {}
  };

  return std::make_shared<MakeSharedEnabler>(uri, std::move(*admin_token));
}

Api::Expected<Admin>
Api::GetCurrentAdmin() const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/admin"s, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<Admin>(*response);
}

Api::Expected<Admin>
Api::CreateAdmin(const Admin& admin) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string create_admin_request;

  if (glz::write_json(admin, create_admin_request)) {
    throw std::runtime_error{fmt::format(fmt::runtime("error JSONify user to send request"))};
  }

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/admin"s, create_admin_request, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<Admin>(*response);
}

Api::Expected<Admin>
Api::ModifyAdmin(const std::string& username, const Admin& admin) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string modify_admin_request;

  if (glz::write_json(admin, modify_admin_request)) {
    throw std::runtime_error{fmt::format(fmt::runtime("error JSONify user to send request"))};
  }

  HttpClient http_client;
  const auto response = http_client.Put(uri_ + "/api/admin/"s + username, modify_admin_request, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<Admin>(*response);
}

Api::Expected<Admin>
Api::RemoveAdmin(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Delete(uri_ + "/api/admin/"s + username, {}, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<Admin>(*response);
}

Api::Expected<Admins>
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

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<Admins>(*response);
}

Api::Expected<System>
Api::GetSystemStats() const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/system/"s, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<System>(*response);
}

Api::Expected<Inbounds>
Api::GetInbounds() const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/inbounds/"s, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<Inbounds>(*response);
}

Api::Expected<Hosts>
Api::GetHosts() const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/hosts/"s, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<Hosts>(*response);
}

Api::Expected<Hosts>
Api::ModifyHosts(const Hosts& hosts) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string modify_hosts_request;

  if (glz::write_json(hosts, modify_hosts_request)) {
    throw std::runtime_error{fmt::format(fmt::runtime("error JSONify user to send request"))};
  }

  HttpClient http_client;
  const auto response = http_client.Put(uri_ + "/api/hosts/"s, modify_hosts_request, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<Hosts>(*response);
}

Api::Expected<User>
Api::AddUser(const User& user) const {
  if (!user.username.has_value()) {
    throw std::runtime_error{"'username' field must be set"};
  }

  const auto is_valid_status =
    user.status.has_value() &&
    (*user.status == status_values::kActive || *user.status == status_values::kOnHold);

  if (!is_valid_status) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("user status must be '{}' or '{}'"),
        status_values::kActive,
        status_values::kOnHold)};
  }

  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string json_request;

  if (glz::write_json(user, json_request)) {
    throw std::runtime_error{fmt::format(fmt::runtime("error JSONify user to send request"))};
  }

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/user/"s, json_request, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<User>(*response);
}

Api::Expected<User>
Api::GetUser(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Get(uri_ + "/api/user/"s + username, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<User>(*response);
}

Api::Expected<User>
Api::ModifyUser(const std::string& username, const User& modified_user) const {
  if (!modified_user.username) {
    throw std::runtime_error{"'username' field must be set"};
  }

  HttpHeaders headers;
  headers.Add("Content-Type", "application/json");
  headers.Add("Authorization", token_type_ + " " + access_token_);

  std::string json_request;

  if (glz::write_json(modified_user, json_request)) {
    throw std::runtime_error{fmt::format(fmt::runtime("error JSONify user to send request"))};
  }

  HttpClient http_client;
  const auto response = http_client.Put(uri_ + "/api/user/"s + username, json_request, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<User>(*response);
}

HttpClient::Response
Api::RemoveUser(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Delete(uri_ + "/api/user/"s + username, {}, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error for '{}': {}"),
        __FUNCTION__,
        username,
        response.error().Description())};
  }

  return *response;
}

Api::Expected<User>
Api::ResetUserDataUsage(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/user/"s + username + "/reset", {}, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error for '{}': {}"),
        __FUNCTION__,
        username,
        response.error().Description())};
  }

  return ParseResponse<User>(*response);
}

Api::Expected<User>
Api::RevokeUserSubscription(const std::string& username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/user/"s + username + "/revoke_sub", {}, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error for '{}': {}"),
        __FUNCTION__,
        username,
        response.error().Description())};
  }

  return ParseResponse<User>(*response);
}

Api::Expected<Users>
Api::GetUsers(GetUsersParams params) const {
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

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<Users>(*response);
}

HttpClient::Response
Api::ResetUsersDataUsage() const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Post(uri_ + "/api/users/reset"s, {}, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return *response;
}

Api::Expected<UserUsage>
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

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<UserUsage>(*response);
}

Api::Expected<User>
Api::SetOwner(const std::string& username, const std::string& admin_username) const {
  HttpHeaders headers;
  headers.Add("Authorization", token_type_ + " " + access_token_);

  HttpClient http_client;
  const auto response = http_client.Put(uri_ + "/api/user/"s + username + "/set-owner/?admin_username=" + admin_username, {}, headers);

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<User>(*response);
}

Api::Expected<UserList>
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

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<UserList>(*response);
}

Api::Expected<UserList>
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

  if (!response) {
    throw std::runtime_error{
      fmt::format(
        fmt::runtime("{}: network error: {}"),
        __FUNCTION__,
        response.error().Description())};
  }

  return ParseResponse<UserList>(*response);
}

Api::Api(std::string uri, std::string token_type, std::string access_token)
    : uri_{std::move(uri)},
      token_type_{std::move(token_type)},
      access_token_{std::move(access_token)} {}

Api::~Api() = default;

}// namespace marzbanpp
